// CdnSource.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnSource.h"
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
            boost::asio::io_service & io_svc, 
            Scheduler & scheduler, 
            Url const & url)
            : Source(scheduler)
            , url_(url)
            , http_(io_svc)
        {
            //io_svc.post(
            //    boost::bind(&Source::on_ready, this));
        }

        CdnSource::~CdnSource()
        {
        }

        bool CdnSource::request(
            std::vector<boost::uint64_t> & pieces)
        {
            if (pieces.empty())
                return true;
            boost::uint64_t seg = SEGMENT(pieces[0]);
            util::protocol::http_field::Range range;
            boost::uint64_t b = 0;
            boost::uint64_t e = 0;
            for (size_t i = 0; i < pieces.size(); ++i) {
                boost::uint64_t pic = BLOCK_PIECE(pieces[0]);
                if (pic == e) {
                    ++e;
                } else {
                    assert(pic > e);
                    if (e > b)
                        range.add_range(b * PIECE_SIZE, e * PIECE_SIZE);
                    b = pic;
                    e = pic + 1;
                }
            }
            if (e > b)
                range.add_range(b * PIECE_SIZE, e * PIECE_SIZE);

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
            if (ec && ec != boost::asio::error::eof)
                return;

            if (piece) {
                ((PoolPiece &)*piece).set_size((boost::uint16_t)bytes_read);
                on_data(pieces_[index_], piece);
                ++index_;
                if (index_ >= pieces_.size())
                    return;
            }

            piece = PoolPiece::alloc();
            boost::asio::async_read(
                http_.response_stream(), 
                boost::asio::buffer(piece->data(), piece->size()), 
                boost::asio::transfer_all(), 
                boost::bind(&CdnSource::handle_read, this, _1, _2, piece));
        }

    } // namespace client
} // namespace trip
