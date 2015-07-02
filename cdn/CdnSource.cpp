// CdnSource.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/cdn/CdnTunnel.h"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/PoolPiece.h"

#include <framework/string/Format.h>

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>

namespace trip
{
    namespace client
    {

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
            on_ready();
            return true;
        }

        bool CdnSource::close()
        {
            return true;
        }

        bool CdnSource::do_request(
            std::vector<DataId> const & pieces)
        {
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
            url.param("seg", framework::string::format(seg));
            util::protocol::HttpRequestHead head;
            head.host = url.host_svc();
            head.path = url.path_all();
            http_.async_open(head, boost::bind(
                    &CdnSource::handle_open, this, _1));
            return true;
        }

        void CdnSource::handle_open(
            boost::system::error_code ec)
        {
            if (ec)
                return;

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
                    if (ranges_.empty())
                        return;
                }
            }

            piece_ = PoolPiece::alloc();
            boost::asio::async_read(
                http_.response_stream(), 
                boost::asio::buffer(piece->data(), piece->size()), 
                boost::asio::transfer_all(), 
                boost::bind(&CdnSource::handle_read, this, _1, _2, piece_));
        }

    } // namespace client
} // namespace trip
