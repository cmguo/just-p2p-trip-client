// Bus.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Bus.h"

#define SHARER_QUEUE_SIZE 16

#define SLOT_ACTIVE 1
#define SLOT_WAIT   2
#define SLOT_SIGNAL 4

#define TIME_WAIT 600  // seconds

namespace trip
{
    namespace client
    {

        Bus::Bus(
            Bus * bus, 
            Queue * queue)
            : Cell(bus, queue)
            , next_id_(0)
        {
        }

        bool Bus::add(
            Cell * cell)
        {
            boost::uint16_t id(slot_alloc());
            slot_attach(id, cell);
            cell->l_id(id);
            return true;
        }

        bool Bus::del(
            Cell * cell)
        {
            boost::uint16_t id(cell->l_id());
            slot_detach(id, cell);
            return true;
        }

        boost::uint16_t Bus::count() const
        {
            return boost::uint16_t(array_.size() - tmwait_slots_.size());
        }

        bool Bus::empty() const
        {
            return Cell::empty() && signal_slots_.empty();
        }

        void Bus::on_send(
            boost::uint16_t id, 
            NetBuffer & buf)
        {
            Slot * slot(slot_at(id));
            assert(slot->cell);
            slot->cell->on_send(buf);
        }

        void Bus::on_send(
            Cell * c, 
            NetBuffer & buf)
        {
            do {
                c->Cell::on_send(buf);
            } while (c->bus() != this && (c = c->bus()));
        }

        void Bus::on_recv(
            boost::uint16_t id, 
            NetBuffer & buf)
        {
            Slot * slot(slot_at(id));
            if (slot->cell)
                slot->cell->on_recv(buf);
        }

        void Bus::signal(
            Cell * cell)
        {
            assert(!cell->empty());
            Slot & slot(slot_at(cell));
            bool s = signal_slots_.empty();
            if ((slot.flags & Slot::sfSignal) == 0) {
                slot.flags |= Slot::sfSignal;
                signal_slots_.push_back(cell->l_id());
                if (s)
                    signal();
            }
        }

        void Bus::send_signal(
            NetBuffer & buf)
        {
            if (!signal_slots_.empty()) {
                Slot * slot = slot_at(signal_slots_.front());
                signal_slots_.pop_front();
                assert(slot->flags & Slot::sfSignal);
                slot->cell->on_send(buf);
                if (slot->cell->empty()) {
                    slot->flags &= ~Slot::sfSignal;
                } else {
                    signal_slots_.push_back(slot->cell->l_id());
                }
            } else if (queue_) {

            }
        }

        void Bus::on_send(
            NetBuffer & buf)
        {
            send_signal(buf);
            Cell::on_send(buf);
        }

        void Bus::on_recv(
            NetBuffer & buf)
        {
            Cell::on_recv(buf);
        }

        bool Bus::is_signal() const
        {
            return !signal_slots_.empty();
        }

        void Bus::on_timer(
            Time const & now)
        {
            Cell::on_timer(now);
            array_t::value_const_iterator iter = array_.value_cbegin();
            for (; iter != array_.value_cend(); ++iter) {
                if (iter->flags & Slot::sfActive)
                    iter->cell->on_timer(now);
            }
            while (!tmwait_slots_.empty() && time_base_ < now) {
                boost::uint16_t id = tmwait_slots_.front();
                tmwait_slots_.pop_front();
                Slot * s = slot_at(id);
                time_base_ += Duration::seconds(s->timeo);
                //s->flags = 0; // slot_free will reset all
                s->timeo = 0;
                slot_free(id);
            }
            if (count() == 0)
                delete this;
        }

        Bus::Slot & Bus::slot_at(
            Cell * cell) const
        {
            Slot * slot(slot_at(cell->l_id()));
            assert(slot->cell == cell);
            return *slot;
        }

        Bus::Slot * Bus::slot_at(
            boost::uint16_t id) const
        {
            return const_cast<Slot *>(&const_cast<array_t const &>(array_).at(id));
        }

        boost::uint16_t Bus::slot_alloc()
        {
            if (array_.size() < 0xffff) {
                Slot * s = &array_[next_id_++];
                while (!(*s == Slot())) {
                    s = &array_[next_id_++];
                }
                s->flags = Slot::sfUsed;
                return next_id_ - 1;
            } else if (!tmwait_slots_.empty()) {
                boost::uint16_t id = tmwait_slots_.front();
                Slot * s = slot_at(id);
                tmwait_slots_.pop_front();
                time_base_ += Duration::seconds(s->timeo);
                s->timeo = 0;
                return id;
            } else {
                assert(false);
                return 0;
            }
        }

        void Bus::slot_free(
            boost::uint16_t id)
        {
            array_.reset(id);
        }

        void Bus::slot_attach(
            boost::uint16_t id, 
            Cell * cell)
        {
            Slot & slot(*slot_at(id));
            slot.flags |= Slot::sfActive;
            slot.cell = cell;
        }

        void Bus::slot_detach(
            boost::uint16_t id, 
            Cell * cell)
        {
            Slot & slot(*slot_at(id));
            assert(slot.cell == cell);
            if (slot.flags & Slot::sfSignal) {
                // TODO: remove from signal_slots_
            }
            Time expire = Time::now() + Duration::seconds(TIME_WAIT);
            slot.flags = Slot::sfUsed;
            slot.timeo = 0;
            slot.cell = NULL;
            if (tmwait_slots_.empty()) {
                time_base_ = time_tail_ = expire;
            } else {
                Slot * s = slot_at(tmwait_slots_.back());
                s->timeo = (boost::uint16_t)(expire - time_tail_).total_seconds();
                time_tail_ += Duration::seconds(s->timeo); // may not equal with exipre for second accuracy
            }
            tmwait_slots_.push_back(id);
        }

        void Bus::foreach_cell(
            cell_visitor_t const & visitor)
        {
            array_t::value_const_iterator iter = array_.value_cbegin();
            for (; iter != array_.value_cend(); ++iter) {
                if (iter->flags & Slot::sfActive)
                    visitor(*iter->cell);
            }
        }

    } // namespace client
} // namespace trip
