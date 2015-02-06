// Priority.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Priority.h"

#include <algorithm>

#define SHARER_QUEUE_SIZE 16

namespace trip
{
    namespace client
    {

        Priority::Priority(
            prio_func_t fprio,
            size_t limit)
            : fprio_(fprio)
            , limit_(limit == 0 ? SHARER_QUEUE_SIZE : limit)
            , count_(0)
        {
            pkts_.resize(limit_);
        }

        struct comp_prio
        {
            comp_prio(
                Priority::prio_func_t fprio)
                : fprio_(fprio)
            {
            }
            bool operator()(
                void * l, 
                void * r) 
            {
                return fprio_(l) < fprio_(r);
            }
            Priority::prio_func_t fprio_;
        };

        bool Priority::push(
            void * pkt)
        {
            if (count_ < limit_) {
                pkts_[count_] = pkt;
                ++count_;
                std::push_heap(pkts_.begin(), pkts_.begin() + count_, comp_prio(fprio_));
                return true;
            } else {
                return false;
            }
        }

        void * Priority::first()
        {
            return count_ == 0 ? NULL : pkts_[0];
        }

        void Priority::pop()
        {
            assert(count_ > 0);
            std::pop_heap(pkts_.begin(), pkts_.begin() + count_, comp_prio(fprio_));
            --count_;
        }

        void Priority::on_timer()
        {
        }

    } // namespace client
} // namespace trip
