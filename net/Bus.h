// Bus.h

#ifndef _TRIP_CLIENT_NET_BUS_H_
#define _TRIP_CLIENT_NET_BUS_H_

#include "trip/client/net/Cell.h"

#include <framework/container/SparseArray.h>

#include <boost/function.hpp>

namespace trip
{
    namespace client
    {

        class Bus
            : public Cell
        {
        public:
            Bus(
                Bus * bus = NULL, 
                Queue * queue = NULL);

        public:
            bool add(
                Cell * cell);

            bool del(
                Cell * cell);

            void signal(
                Cell * cell);

            boost::uint16_t count() const;

            typedef boost::function<void (Cell &)> cell_visitor_t;

            void foreach_cell(
                cell_visitor_t const & visitor);

            std::deque<boost::uint16_t> const & signal_slots()
            {
                return signal_slots_;
            }

            std::deque<boost::uint16_t> const & tmwait_slots()
            {
                return tmwait_slots_;
            }

        public:
            virtual void on_send(
                NetBuffer & buf);

            virtual void on_recv(
                NetBuffer & buf);

            virtual void on_timer(
                Time const & now);

            virtual bool empty() const;

        protected:
            struct Slot;

            struct Slot
            {
                enum Flags
                {
                    sfUsed      = 1, 
                    sfActive    = 2, 
                    sfSignal    = 4, 
                };

                Slot()
                {
                    flags = 0;
                    timeo = 0;
                    cell = NULL;
                }

                friend bool operator==(
                    Slot const & l, 
                    Slot const & r)
                {
                    return l.flags == r.flags && l.cell == r.cell;
                }

                boost::uint16_t flags;
                boost::uint16_t timeo;
                Cell * cell;
            };

        protected:
            void on_send(
                boost::uint16_t id, 
                NetBuffer & buf);

            void on_send(
                Cell * c, 
                NetBuffer & buf);

            void on_recv(
                boost::uint16_t id, 
                NetBuffer & buf);

            using Cell::signal;

            Slot & slot_at(
                Cell * cell) const;

            Slot * slot_at(
                boost::uint16_t id) const;

            bool is_signal() const;

            void send_signal(
                NetBuffer & buf);

        private:
            boost::uint16_t slot_alloc();

            void slot_free(
                boost::uint16_t id); // real free

            void slot_attach(
                boost::uint16_t id, 
                Cell * cell);

            void slot_detach(
                boost::uint16_t id, 
                Cell * cell); // for check

        protected:
            typedef framework::container::SparseArray<Slot> array_t;
            array_t array_;
            boost::uint16_t next_id_;
            std::deque<boost::uint16_t> signal_slots_;
            std::deque<boost::uint16_t> tmwait_slots_;
            Time time_base_;
            Time time_tail_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_BUS_H_

