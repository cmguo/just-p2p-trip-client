// Cell.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Cell.h"
#include "trip/client/net/Bus.h"
#include "trip/client/net/Queue.h"

namespace trip
{
    namespace client
    {

        Cell::Cell(
            Bus * bus, 
            Queue * queue)
            : id_(0)
            , bus_(bus)
            , queue_(queue)
        {
            if (bus_)
                bus_->add(this);
        }

        Cell::~Cell()
        {
            if (bus_)
                bus_->del(this);
            if (queue_)
                delete queue_;
        }

        void Cell::on_send(
            void * head, 
            NetBuffer & buf)
        {
        }

        void Cell::on_recv(
            void * head, 
            NetBuffer & buf)
        {
        }

        void Cell::on_timer(
            Time const & now)
        {
        }

        bool Cell::empty() const
        {
            return queue_ == NULL || queue_->empty();
        }

        void Cell::signal()
        {
            if (bus_)
                bus_->signal(this);
        }
 
        bool Cell::push(
            void * pkt)
        {
            if (queue_) {
                if (queue_->push(pkt)) {
                    signal();
                    return true;
                } else {
                    return false;
                }
            } else if (bus_) {
                return bus_->push(pkt);
            } else {
                return false;
            }
        }

        void * Cell::first()
        {
            return queue_->first();
        }

        void Cell::pop()
        {
            queue_->pop();
        }
    } // namespace client
} // namespace trip
