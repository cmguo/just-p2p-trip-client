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

            void active(
                Cell * cell);

            void inactive(
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

            virtual void on_timer();

        protected:
            struct Slot
            {
                Slot(
                    Cell * cell)
                    : next(0)
                    , flag(0)
                    , cell(cell)
                {
                }
                size_t next;
                size_t flag;
                Cell * cell;
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

        protected:
            std::vector<Slot> slots_;
            size_t free_slots_;
            size_t ready_slots_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_BUS_H_

