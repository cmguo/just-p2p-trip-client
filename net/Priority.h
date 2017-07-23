// Priority.h

#ifndef _TRIP_CLIENT_NET_PRIORITY_H_
#define _TRIP_CLIENT_NET_PRIORITY_H_

#include "trip/client/net/Queue.h"

namespace trip
{
    namespace client
    {

        class Priority
        {
        public:
            typedef size_t (* prio_func_t)(void *);

        public:
            Priority(
                prio_func_t fprio, 
                size_t limit = 0);

        public:
            bool push(
                void * p);

            void * first();

            void pop();

            void on_timer();

            size_t size() const;

        private:
            prio_func_t fprio_;
            size_t limit_;
            size_t count_;
            std::vector<void *> pkts_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_PRIORITY_H_

