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
            : l_id_(0)
            , p_id_(0)
            , bus_(NULL)
            , queue_(queue)
        {
            attach(bus);
        }

        Cell::~Cell()
        {
            detach();
            if (queue_)
                delete queue_;
        }

        void Cell::attach(
            Bus * bus)
        {
            bus_ = bus;
            if (bus_)
                bus_->add(this);
        }

        void Cell::detach()
        {
            if (bus_)
                bus_->del(this);
            bus_ = NULL;
        }

        static boost::uint32_t in_avail(
            NetBuffer & buf)
        {
            return buf.pubseekoff(0, std::ios::cur, std::ios::out)
                - buf.pubseekoff(0, std::ios::cur, std::ios::in);
        }

        void Cell::on_send(
            //void * head, 
            NetBuffer & buf)
        {
            stat_.push_send(in_avail(buf));
        }

        void Cell::on_recv(
            //void * head, 
            NetBuffer & buf)
        {
            stat_.push_recv(in_avail(buf));
        }

        void Cell::on_timer(
            Time const & now)
        {
            stat_.on_timer(now);
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
