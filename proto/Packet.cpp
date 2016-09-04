// Packet.cpp

#include "trip/client/Common.h"
#include "trip/client/proto/Packet.h"
#include "trip/client/proto/PacketBuffers.h"
#include "trip/client/core/PoolPiece.h"

#include <iostream>

namespace trip
{
    namespace client
    {

        Packet::Packet()
            : PacketBuffers(*this)
        {
            body_ = PoolPiece::alloc();
        }

        Packet::~Packet()
        {
        }

        bool Packet::swap_body(
            Piece::pointer & piece, 
            bool read)
        {
            if (read) {
                if (in_position() != HEAD_SIZE)
                    return false;
                boost::uint16_t size = 
                    (boost::uint16_t)in_avail();
                body_->set_size(size);
                consume(size);
            } else {
                if (out_position() != HEAD_SIZE)
                    return false;
                commit(piece->size());
            }
            piece.swap(body_);
            return true;
        }

        boost::uint16_t Packet::checksum() const
        {
            boost::uint32_t sum = 0;
            if (in_avail() & 1) {
                *pptr() = 0; // pad one byte
            }
            const_byte_buffers const cdata(data());
            for (const_byte_buffers::const_iterator iter = cdata.begin(); iter != cdata.end(); ++iter) {
                boost::uint16_t const * ptr = 
                    boost::asio::buffer_cast<boost::uint16_t const *>(*iter);
                size_t size = (boost::asio::buffer_size(*iter) + 1) / 2;
                while (size) {
                    sum += *ptr;
                    ++ptr;
                    --size;
                }
            }
            while (sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);
            return (boost::uint16_t)~sum;
        }

        void PacketBufferIterator::increment()
        {
            void * p = 
                boost::asio::buffer_cast<void *>(buf_);
            size_t n = 0;
            if (p == NULL) {
                p = pkt_->head_;
                n = Packet::HEAD_SIZE;
            } else if (p == pkt_->head_) {
                p = pkt_->body_->data();
                n = PIECE_SIZE;
            } else if (p == pkt_->body_->data()) {
                p = pkt_->tail_;
                n = Packet::TAIL_SIZE;
            } else {
                p = NULL;
                n = 0;
            }
            buf_ = boost::asio::buffer(p, n);
        }

    } // namespace client
} // namespace trip
