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
            void * pkt)
        {
            if (pkts_.size() < limit_) {
                pkts_.push_back(pkt);
                return true;
            } else {
                return false;
            }
        }

        void * Fifo::first()
        {
            return pkts_.empty() ? NULL : pkts_.front();
        }

        void Fifo::pop()
        {
            assert(!pkts_.empty());
            pkts_.pop_front();
        }

    } // namespace client
} // namespace trip
