// Fifo.h

#ifndef _TRIP_CLIENT_NET_FIFO_H_
#define _TRIP_CLIENT_NET_FIFO_H_

#include "trip/client/net/Queue.h"

namespace trip
{
    namespace client
    {

        class Fifo
            : public Queue
        {
        public:
            Fifo(
                size_t limit = 0);

        public:
            bool push(
                void * pkt);

            void * first();

            void pop();

        protected:
            size_t limit_;
            std::deque<void *> pkts_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_FIFO_H_

