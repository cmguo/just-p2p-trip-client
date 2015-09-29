// Queue.h

#ifndef _TRIP_CLIENT_NET_QUEUE_H_
#define _TRIP_CLIENT_NET_QUEUE_H_

namespace trip
{
    namespace client
    {

        class Queue
        {
        public:
            virtual ~Queue() {}

            virtual bool push(
                void * p) = 0;

            virtual void * first() const = 0;

            virtual void pop() = 0;

            virtual void on_timer() {};
            
            virtual bool empty() const { return first() == NULL; }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_QUEUE_H_

