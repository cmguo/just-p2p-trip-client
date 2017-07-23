// Cell.h

#ifndef _TRIP_CLIENT_NET_SLOT_H_
#define _TRIP_CLIENT_NET_SLOT_H_

#include "trip/client/net/Stat.h"

#include <streambuf>

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
                Bus * bus = NULL, 
                Queue * queue = NULL);

            virtual ~Cell();
                
        public:
            void attach(
                Bus * bus);

            void detach();

            Bus * bus() const;

        public:
            boost::uint16_t l_id() const
            {
                return l_id_;
            }

            void l_id(
                boost::uint16_t n)
            {
                l_id_ = n;
            }

            boost::uint16_t p_id() const
            {
                return p_id_;
            }

            void p_id(
                boost::uint16_t n)
            {
                p_id_ = n;
            }

            Stat const & stat() const
            {
                return stat_;
            }

        public:
            virtual void on_send(
                //void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                //void * head, 
                NetBuffer & buf);

            virtual void on_timer(
                Time const & now);

            virtual bool empty() const;

            Queue * queue() const
            {
                return queue_;
            }

            bool push(
                void * pkt);

        protected:
            void signal();

            bool push(
                Cell * c, 
                void * p);

            void * first(
                Cell *& c);

            void pop();

        protected:
            boost::uint16_t l_id_;
            boost::uint16_t p_id_;
            Bus * bus_;
            Queue * queue_;
            Stat stat_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_SLOT_H_

