// CdnSource.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/M3u8Media.h"
#include "trip/client/ssp/SspTunnel.h"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/Segment.h"

#include <util/archive/ArchiveBuffer.h>

#include <framework/string/Format.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CdnSource", framework::logger::Debug);

        CdnSource::CdnSource(
            CdnManager & manager, 
            SspTunnel & tunnel, 
            Resource & resource, 
            Url const & url)
            : Source(resource, url)
            , SspSession(tunnel)
            , manager_(manager)
            , http_(tunnel.io_svc())
            , m3u8_(NULL)
            , m3u8_is_open_(false)
        {
            //io_svc.post(
            //    boost::bind(&Source::on_ready, this));
        }

        CdnSource::~CdnSource()
        {
            manager_.del_source(this);
            if (m3u8_)
                delete m3u8_;
        }

        bool CdnSource::open(
            Url const & url)
        {
            LOG_DEBUG("[open] url=" << url.to_string());
            if (url.path()[url.path().size() - 1] == '/') {
                on_ready();
            } else {
                m3u8_ = new M3u8Media(tunnel().io_svc(), url);
                m3u8_->async_open(
                    boost::bind(&CdnSource::handle_m3u8_open, this, _1));
            }
            return true;
        }

        bool CdnSource::close()
        {
            LOG_DEBUG("[close]");
            return true;
        }

        bool CdnSource::request(
            std::vector<DataId> const & pieces)
        {
            LOG_TRACE("[request]");
            assert(!piece_);
            if (piece_)
                return false;
            if (pieces.empty())
                return true;
            boost::uint64_t segno = pieces[0].segment;
            PieceRange r;
            SegmentMeta const * meta = resource().get_segment_meta(pieces[0]);
            r.b = pieces[0];
            r.e = pieces[0];
            for (size_t i = 0; i < pieces.size(); ++i) {
                DataId pic = pieces[i];
                if (pic == r.e) {
                    r.e.inc_piece();
                } else {
                    assert(pic > r.e);
                    assert(r.e > r.b);
                    ranges_.push_back(r);
                    r.b = pic;
                    pic.inc_piece();
                    r.e = pic;
                }
            }
            assert(r.e > r.b);
            if (meta != NULL && meta->bytesize < (boost::uint32_t)r.e.block_piece * PIECE_SIZE)
                r.e.inc_segment(); // mark range to segment end
            ranges_.push_back(r);

            Url url;
            segment_url(url, segno);
            util::protocol::HttpRequestHead head;
            head.host = url.host_svc();
            head.path = url.path_all();
            make_range(head);
            LOG_TRACE("[request] segno=" << segno << ", range=" << head.range.get().to_string());
            //head.get_content(std::cout);
            piece_ = PoolPiece::alloc();
            http_.async_open(head, boost::bind(
                    &CdnSource::handle_open, this, _1));
            return true;
        }

        void CdnSource::cancel()
        {
            LOG_TRACE("[cancel]");
            if (piece_)
                piece_->set_size(0);
            ranges_.clear();
            boost::system::error_code ec;
            http_.cancel(ec);
            if (m3u8_)
                m3u8_->cancel(ec);
        }

        bool CdnSource::has_segment(
            DataId sid) const
        {
            return true;
        }

        bool CdnSource::has_block(
            DataId bid) const
        {
            return false;
        }

        boost::uint32_t CdnSource::window_size() const
        {
            return PIECE_PER_BLOCK;
        }

        boost::uint32_t CdnSource::window_left() const
        {
            if (m3u8_ && !m3u8_is_open_)
                return 0;
            if (piece_)
                return 0;
            return window_size();
        }

        void CdnSource::segment_url(
            Url & url,
            size_t segno)
        {
            if (m3u8_ == NULL) {
                url = url_;
                url.path(url.path() + framework::string::format(segno) + resource_.meta()->file_extension);
            } else {
                boost::system::error_code ec;
                m3u8_->segment_url(segno, url, ec);
            }
        }
 
        void CdnSource::handle_m3u8_open(
            boost::system::error_code ec)
        {
            if (!ec) {
                m3u8_is_open_ = true;
                on_ready();
            }
        }

        void CdnSource::handle_open(
            boost::system::error_code ec)
        {
            // recv 0 byte to update time
            util::archive::ArchiveBuffer<boost::uint8_t> buf((boost::uint8_t *)NULL, 0, 0);
            SspSession::on_recv(buf);

            //SspSession::on_send();
            //SspSession::on_recv();

            if (ranges_.empty()) {
                LOG_DEBUG("[handle_open] segno=???" 
                    << ", range=" << http_.request().head().range.get().to_string() 
                    << ", ec=canceled");
                http_.close(ec);
                piece_.reset();
                on_ready();
                return;
            }

            if (ec) {
                LOG_WARN("[handle_open] segno=" << ranges_.front().b.segment 
                    << ", range=" << http_.request().head().range.get().to_string() 
                    << ", ec=" << ec.message());
                util::protocol::HttpRequestHead head(http_.request().head());
                http_.close(ec);
                piece_ = PoolPiece::alloc();
                //return;
                http_.async_open(head, boost::bind(
                        &CdnSource::handle_open, this, _1));
                return;
            }

            //http_.response().head().get_content(std::cout);

            SegmentMeta meta;
            meta.bytesize = http_.response().head().content_range.get().total();
            //assert(http_.request().head().range.get()[0] == http_.response().head().content_range.get().unit());
            on_segment_meta(ranges_.front().b, meta);

            boost::asio::async_read(
                http_.response_stream(), 
                boost::asio::buffer(piece_->data(), piece_->size()), 
                boost::asio::transfer_all(), 
                boost::bind(&CdnSource::handle_read, this, _1, _2));
        }

        void CdnSource::handle_read(
            boost::system::error_code ec, 
            size_t bytes_read)
        {
            util::archive::ArchiveBuffer<boost::uint8_t> buf((boost::uint8_t *)NULL, bytes_read, bytes_read);
            SspSession::on_recv(buf);

            Piece::pointer piece;
            piece.swap(piece_);

            if (piece->size() == 0) {// canceled
                LOG_TRACE("[handle_read] segno=" << ranges_.front().b.segment 
                    << ", range=" << http_.request().head().range.get().to_string() 
                    << ", ec=canceled");
                util::protocol::HttpRequestHead head(http_.request().head());
                http_.close(ec);
                if (ranges_.empty()) {
                    on_ready();
                } else {
                    make_range(head);
                    piece_ = PoolPiece::alloc();
                    http_.async_open(head, boost::bind(
                            &CdnSource::handle_open, this, _1));
                }
                return;
            }

            if (ranges_.empty()) {
                LOG_TRACE("[handle_read] segno=???" 
                    << ", range=" << http_.request().head().range.get().to_string() 
                    << ", ec=canceled");
                http_.close(ec);
                on_ready();
                return;
            }

            if (bytes_read == 0) {
                LOG_WARN("[handle_read] segno=" << ranges_.front().b.segment 
                    << ", range=" << http_.request().head().range.get().to_string() 
                    << ", ec=" << ec.message());
                util::protocol::HttpRequestHead head(http_.request_head());
                http_.close(ec);
                make_range(head);
                piece_ = PoolPiece::alloc();
                http_.async_open(head, boost::bind(
                    &CdnSource::handle_open, this, _1));
                return;
            }

            PieceRange & r = ranges_.front();

            piece->set_size((boost::uint16_t)bytes_read);
            on_data(r.b, piece);
            piece.reset();

            r.b.inc_piece();
            if (r.b.block_piece == r.e.block_piece) {
                DataId i(r.b);
                ranges_.pop_front();
                if (ranges_.empty()) {
                    LOG_TRACE("[handle_read] segno=" << i.segment 
                        << ", range=" << http_.request().head().range.get().to_string() 
                        << ", ec=finished");
                    http_.close(ec);
                    on_ready();
                    return;
                } else {
                    // next range
                    // TODO: support multi-range
                    util::protocol::HttpRequestHead head = http_.request_head();
                    make_range(head);
                    //head.get_content(std::cout);
                    http_.close(ec);
                    piece_ = PoolPiece::alloc();
                    http_.async_open(head, boost::bind(
                            &CdnSource::handle_open, this, _1));
                    return;
                }
            }

            piece_ = PoolPiece::alloc();

            boost::asio::async_read(
                http_.response_stream(), 
                boost::asio::buffer(piece_->data(), piece_->size()), 
                boost::asio::transfer_all(), 
                boost::bind(&CdnSource::handle_read, this, _1, _2));
        }

        void CdnSource::on_timer(
            Time const & now)
        {
            Cell::on_timer(now);
            if (ranges_.empty())
                return;
            if (now >= stat_.recv_bytes().time + Duration::milliseconds(1500) && 
                now >= stat_.send_bytes().time + Duration::milliseconds(1500)) {
                if (piece_) {
                    PieceRange & r = ranges_.front();
                    LOG_WARN("[on_timer] segno=" << r.b.segment 
                        << ", range=" << http_.request().head().range.get().to_string()
                        << ", ec=timeout");
                    piece_->set_size(0); // cancel
                    boost::system::error_code ec;
                    http_.cancel(ec);
                }
            }
        }

        void CdnSource::make_range(
            util::protocol::HttpHead & head)
        {
            // send 0 byte to update time
            util::archive::ArchiveBuffer<boost::uint8_t> buf((boost::uint8_t *)NULL, 0, 0);
            SspSession::on_send(buf);

            head.range.reset(util::protocol::http_field::Range());
            util::protocol::http_field::Range & range = head.range.get();
            // TODO: support multi-range
            //for (size_t i = 0; i < ranges_.size(); ++i) {
            //    PieceRange & r(ranges_[i]);
            //    range.put(r.b.block_piece * PIECE_SIZE, r.e.block_piece * PIECE_SIZE);
            //}
            PieceRange & r(ranges_.front());
            if (r.e.segment != r.b.segment)
                range.put(r.b.block_piece * PIECE_SIZE);
            else
                range.put(r.b.block_piece * PIECE_SIZE, r.e.block_piece * PIECE_SIZE);
        }

    } // namespace client
} // namespace trip
