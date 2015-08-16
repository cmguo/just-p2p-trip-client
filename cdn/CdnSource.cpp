// CdnSource.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/cdn/CdnTunnel.h"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/Segment.h"

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
            CdnTunnel & tunnel, 
            Resource & resource, 
            Url const & url)
            : Source(resource, url)
            , http_(tunnel.io_svc())
        {
            //io_svc.post(
            //    boost::bind(&Source::on_ready, this));
        }

        CdnSource::~CdnSource()
        {
        }

        bool CdnSource::open(
            Url const & url)
        {
            LOG_INFO("[open] url=" << url.to_string());
            on_ready();
            return true;
        }

        bool CdnSource::close()
        {
            LOG_INFO("[close]");
            return true;
        }

        bool CdnSource::request(
            std::vector<DataId> const & pieces)
        {
            LOG_TRACE("[request]");
            if (pieces.empty())
                return true;
            boost::uint64_t seg = pieces[0].segment;
            util::protocol::http_field::Range range;
            PieceRange r;
            r.b = pieces[0];
            r.e = pieces[0];
            for (size_t i = 0; i < pieces.size(); ++i) {
                DataId pic = pieces[i];
                if (pic == r.e) {
                    r.e.inc_piece();
                } else {
                    assert(pic > r.e);
                    if (r.e > r.b) {
                        ranges_.push_back(r);
                        range.add_range(r.b.block_piece * PIECE_SIZE, r.e.block_piece * PIECE_SIZE);
                    }
                    r.b = pic;
                    pic.inc_piece();
                    r.e = pic;
                }
            }
            if (r.e > r.b) {
                ranges_.push_back(r);
                range.add_range(r.b.block_piece * PIECE_SIZE, r.e.block_piece * PIECE_SIZE);
            }

            Url url(url_);
            url.path(url.path() + framework::string::format(seg));
            util::protocol::HttpRequestHead head;
            head.host = url.host_svc();
            head.path = url.path_all();
            head.range = range;
            LOG_INFO("[request] url=" << url.to_string() << ", range=" << range.to_string());
            //head.get_content(std::cout);
            http_.async_open(head, boost::bind(
                    &CdnSource::handle_open, this, _1));
            return true;
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
            return window_size();
        }

        void CdnSource::handle_open(
            boost::system::error_code ec)
        {
            LOG_DEBUG("[handle_open] ec=" << ec.message());
            if (ec)
                return;

            //http_.response().head().get_content(std::cout);

            SegmentMeta meta;
            meta.bytesize = http_.response().head().content_range.get().total();
            on_segment_meta(ranges_.front().b, meta);

            handle_read(ec, 0, NULL);
        }

        void CdnSource::handle_read(
            boost::system::error_code ec, 
            size_t bytes_read, 
            Piece::pointer piece)
        {
            if (piece && piece->nref() == 1) // canceled
                return;

            piece_.reset();

            if (ec && ec != boost::asio::error::eof) {
                return;
            }

            if (piece) {
                ((PoolPiece &)*piece).set_size((boost::uint16_t)bytes_read);
                if (ranges_.empty())
                    return;
                PieceRange & r = ranges_.front();
                on_data(r.b, piece);
                r.b.inc_piece();
                if (r.b == r.e) {
                    ranges_.pop_front();
                    if (ranges_.empty()) {
                        http_.close(ec);
                        on_ready();
                        return;
                    }
                }
            }

            piece_ = PoolPiece::alloc();
            boost::asio::async_read(
                http_.response_stream(), 
                boost::asio::buffer(piece_->data(), piece_->size()), 
                boost::asio::transfer_all(), 
                boost::bind(&CdnSource::handle_read, this, _1, _2, piece_));
        }

        void CdnSource::on_timer(
            Time const & now)
        {
        }

    } // namespace client
} // namespace trip
