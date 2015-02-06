// Bus.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Bus.h"

#define SHARER_QUEUE_SIZE 16

#define SLOT_ACTIVE 1
#define SLOT_WAIT   2
#define SLOT_SIGNAL 4

namespace trip
{
    namespace client
    {

        Bus::Bus(
            Bus * bus, 
            Queue * queue)
            : Cell(bus, queue)
        {
        }

        bool Bus::add(
            Cell * cell)
        {
            cell->id(slots_.size());
            slots_.push_back(Slot(cell));
            return true;
        }

        bool Bus::del(
            Cell * cell)
        {
            assert(slots_[cell->id()].cell == cell);
            slots_[cell->id()].cell = NULL;
            return true;
        }

        void Bus::on_send(
            boost::uint16_t id, 
            void * head, 
            NetBuffer & buf)
        {
            if (id < slots_.size() && (slots_[id].flag & SLOT_ACTIVE)) {
                slots_[id].cell->on_send(head, buf);
            }
        }

        void Bus::on_recv(
            boost::uint16_t id, 
            void * head, 
            NetBuffer & buf)
        {
            if (id < slots_.size() && (slots_[id].flag & SLOT_ACTIVE)) {
                slots_[id].cell->on_recv(head, buf);
            }
        }

        void Bus::active(
            Cell * cell)
        {
            Slot & slot(slot_at(cell));
            assert((slot.flag & SLOT_ACTIVE) == 0);
            slot.flag |= SLOT_ACTIVE;
        }

        void Bus::inactive(
            Cell * cell)
        {
            Slot & slot(slot_at(cell));
            assert(slot.flag & SLOT_ACTIVE);
            slot.flag &= ~SLOT_ACTIVE;
        }

        void Bus::signal(
            Cell * cell)
        {
            Slot & slot(slot_at(cell));
            if (slot.flag & SLOT_SIGNAL)
                return;
            slot.flag |= SLOT_SIGNAL;
            signal();
        }

        void Bus::on_send(
            void * head, 
            NetBuffer & buf)
        {
        }

        void Bus::on_recv(
            void * head, 
            NetBuffer & buf)
        {
        }

        void Bus::on_timer()
        {
        }

        Bus::Slot & Bus::slot_at(
            Cell * cell)
        {
            assert(slots_[cell->id()].cell == cell);
            return slots_[cell->id()];
        }

    } // namespace client
} // namespace trip
