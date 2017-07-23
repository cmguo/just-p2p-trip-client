// Fifo.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Fifo.h"

#define SHARER_QUEUE_SIZE 16

namespace trip
{
    namespace client
    {

        Fifo::Fifo(
            size_t limit)
            : limit_(limit == 0 ? SHARER_QUEUE_SIZE : limit)
        {
        }

        bool Fifo::push(
            Packet const & pkt)
        {
            if (pkts_.size() < limit_) {
                pkts_.push_back(pkt);
                return true;
            } else {
                return false;
            }
        }

        Queue::Packet const * Fifo::first() const
        {
            return pkts_.empty() ? NULL : &pkts_.front();
        }

        void Fifo::pop()
        {
            assert(!pkts_.empty());
            pkts_.pop_front();
        }

        bool Fifo::empty() const
        {
            return pkts_.empty();
        }

        size_t Fifo::size() const
        {
            return pkts_.size();
        }

    } // namespace client
} // namespace trip
