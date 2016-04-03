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
            Cell * c, 
            void * p)
        {
            return queue_->push(c, p);
        }

        void * Shaping::first(Cell *& c)
        {
            return queue_->first(c);
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
