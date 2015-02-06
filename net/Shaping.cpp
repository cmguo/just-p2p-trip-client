// Shaping.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Shaping.h"

#define SHARER_QUEUE_SIZE 16

namespace trip
{
    namespace client
    {

        Shaping::Shaping(
            Queue * queue)
            : queue_(queue)
        {
        }

        bool Shaping::push(
            void * pkt)
        {
            return queue_->push(pkt);
        }

        void * Shaping::first()
        {
            return queue_->first();
        }

        void Shaping::pop()
        {
            queue_->pop();
        }

        void Shaping::on_timer()
        {
            quota_ += quota_delta_;
        }

    } // namespace client
} // namespace trip
