// Cell.h

#ifndef _TRIP_CLIENT_NET_SLOT_H_
#define _TRIP_CLIENT_NET_SLOT_H_

#include <streambuf>

#include <framework/timer/ClockTime.h>

namespace trip
{
    namespace client
    {

        class Bus;
        class Queue;

        typedef std::basic_streambuf<boost::uint8_t> NetBuffer;

        class Cell
        {
        public:
            Cell(
                Bus * bus, 
                Queue * queue = NULL);

            virtual ~Cell();
                
        public:
            boost::uint16_t id() const
            {
                return id_;
            }

            void id(
                boost::uint16_t n)
            {
                id_ = n;
            }

        public:
            virtual void on_send(
                void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                void * head, 
                NetBuffer & buf);

            virtual void on_timer(
                framework::timer::Time const & now);

        protected:
            void signal();

            bool push(
                void * ptk);

            void * first();

            void pop();
            
        protected:
            boost::uint16_t id_;
            Bus * bus_;
            Queue * queue_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_SLOT_H_

