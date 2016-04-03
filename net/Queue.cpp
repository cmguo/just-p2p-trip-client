// Queue.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Queue.h"

namespace trip
{
    namespace client
    {

        bool Queue::push(
            Cell * c, 
            void * p)
        {
            Packet pkt = {c, p};
            return push(pkt);
        }

        void * Queue::first(Cell *& c) const
        {
            Packet const * pkt = first();
            if (pkt == NULL)
                return NULL;
            c = pkt->c;
            return pkt->p;
        }

        bool Queue::empty() const
        {
            return first() == NULL;
        }

        void Queue::on_timer(
            Time const & now)
        {
        }

    } // namespace client
} // namespace trip
