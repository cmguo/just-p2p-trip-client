// Queue.h

#ifndef _TRIP_CLIENT_NET_QUEUE_H_
#define _TRIP_CLIENT_NET_QUEUE_H_

namespace trip
{
    namespace client
    {
        
        class Cell;

        class Queue
        {
        public:
            virtual ~Queue() {}

            virtual bool push(
                Cell * c, 
                void * p);

            virtual void * first(Cell *& c) const;

            virtual void pop() = 0;

            virtual void on_timer(
                Time const & now);
            
            virtual bool empty() const;

        protected:
            struct Packet
            {
                Cell * c;
                void * p;
            };

        protected:
            virtual bool push(
                Packet const & pkt) = 0;

            virtual Packet const * first() const = 0;

        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_QUEUE_H_

