// Packet.cpp

#include "trip/client/Common.h"
#include "trip/client/proto/Packet.h"
#include "trip/client/proto/PacketBuffers.h"
#include "trip/client/core/Piece.h"

namespace trip
{
    namespace client
    {

        Packet::Packet()
        {
            body_ = Piece2::alloc();
        }

        Packet::~Packet()
        {
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
                p = pkt_->body_->data;
                n = PIECE_SIZE;
            } else if (p == pkt_->body_->data) {
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
