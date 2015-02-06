// Shaping.h

#ifndef _TRIP_CLIENT_NET_SHAPING_H_
#define _TRIP_CLIENT_NET_SHAPING_H_

#include "trip/client/net/Queue.h"

namespace trip
{
    namespace client
    {

        class Shaping
            : public Queue
        {
        public:
            Shaping(
                Queue * queue);

        public:
            virtual bool push(
                void * p);

            virtual void * first();

            virtual void pop();

            virtual void on_timer();

        private:
            Queue * queue_;
            boost::uint32_t speed_;
            boost::uint32_t quota_max_;
            boost::uint32_t quota_delta_;
            boost::uint32_t quota_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_SHAPING_H_

