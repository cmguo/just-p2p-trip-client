// Packet.cpp

#include "trip/client/Common.h"
#include "trip/client/proto/Packet.h"
#include "trip/client/proto/PacketBuffers.h"
#include "trip/client/core/PoolPiece.h"

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
            for (const_iterator iter = begin(); iter != end(); ++iter) {
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
            boost::uint8_t * p = 
                boost::asio::buffer_cast<boost::uint8_t *>(buf_);
            size_t n = 0;
            if (p == NULL) {
                p = pkt_->head_;
                n = sizeof(pkt_->head_);
            } else if (p == pkt_->head_) {
                p = pkt_->body_->data();
                n = PIECE_SIZE;
            } else if (p == pkt_->body_->data()) {
                p = pkt_->tail_;
                n = sizeof(pkt_->tail_);
            } else {
                p = NULL;
                n = 0;
            }
            buf_ = boost::asio::buffer(p, n);
        }

    } // namespace client
} // namespace trip
