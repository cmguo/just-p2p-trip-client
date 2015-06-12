// Bus.h

#ifndef _TRIP_CLIENT_NET_BUS_H_
#define _TRIP_CLIENT_NET_BUS_H_

#include "trip/client/net/Cell.h"

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
            struct Slot;
            struct Node;

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
                    next = NULL;
                    flags = 0;
                    cell = NULL;
                }
                Slot * next;
                boost::uint32_t flags;
                union {
                    Cell * cell;
                    Node * node;
                    struct {
                        boost::uint16_t id : 15;
                        boost::uint16_t mark : 1;
                        boost::uint16_t timeo;
                    };
                };
            };

            struct Node
            {
                Slot slots[32];
            };

        protected:
            void on_send(
                boost::uint16_t id, 
                void * head, 
                NetBuffer & buf);

            void on_recv(
                boost::uint16_t id, 
                void * head, 
                NetBuffer & buf);

            using Cell::signal;

            Slot & slot_at(
                Cell * cell);

            Slot * slot_at(
                boost::uint16_t id);

        private:
            Slot * slot_alloc();

            void slot_free(
                boost::uint16_t id); // real free

            void slot_free(
                Slot & slot);

        protected:
            Slot root_;
            boost::uint16_t next_id_;
            Slot * signal_slots_;
            Slot ** signal_slots_tail_;
            Slot * tmwait_slots_;
            Slot ** tmwait_slots_tail_;
            framework::timer::Time time_base_;
            framework::timer::Time time_tail_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_BUS_H_

