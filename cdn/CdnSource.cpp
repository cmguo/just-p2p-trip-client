// CdnSource.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/PoolPiece.h"

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>

namespace trip
{
    namespace client
    {

        CdnSource::CdnSource(
            boost::asio::io_service & io_svc, 
            Resource & resource)
            : Source(resource)
            , http_(io_svc)
        {
        }

        CdnSource::~CdnSource()
        {
        }

        bool CdnSource::request(
            std::vector<boost::uint64_t> & pieces)
        {
            return false;
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
            if (ec)
                return;

            if (piece) {
                assert(piece->size() == (boost::uint16_t)bytes_read);
                if (piece->size() == (boost::uint16_t)bytes_read) {
                    on_data(pieces_[index_], piece);
                    ++index_;
                } else {
                    return;
                }
            }

            piece = PoolPiece::alloc(piece_size(pieces_[index_]));
            boost::asio::async_read(
                http_.response_stream(), 
                boost::asio::buffer(piece->data(), piece->size()), 
                boost::asio::transfer_all(), 
                boost::bind(&CdnSource::handle_read, this, _1, _2, piece));
        }

    } // namespace client
} // namespace trip
