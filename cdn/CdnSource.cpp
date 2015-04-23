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

        bool CdnSource::open()
        {
            on_ready();
            return true;
        }

        bool CdnSource::close()
        {
            return true;
        }

        bool CdnSource::do_request()
        {
            if (requests_.empty())
                return true;
            boost::uint64_t seg = requests_[0].segment;
            util::protocol::http_field::Range range;
            boost::uint64_t b = 0;
            boost::uint64_t e = 0;
            for (size_t i = 0; i < requests_.size(); ++i) {
                boost::uint64_t pic = requests_[i].block_piece;
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
            if (piece && piece->nref() == 1) // canceled
                return;

            piece_.reset();

            if (ec && ec != boost::asio::error::eof) {
                return;
            }

            if (piece) {
                ((PoolPiece &)*piece).set_size((boost::uint16_t)bytes_read);
                on_data(requests_[index_], piece);
                ++index_;
                if (index_ >= requests_.size())
                    return;
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
