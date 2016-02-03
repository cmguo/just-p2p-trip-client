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
            Slot * slot(slot_alloc());
            if (slot == NULL)
                return false;
            cell->l_id(slot->id);
            slot->flags |= Slot::sfActive;
            slot->cell = cell;
            return true;
        }

        bool Bus::del(
            Cell * cell)
        {
            Slot & slot(slot_at(cell));
            slot.flags &= ~Slot::sfActive;
            slot_free(slot);
            if (count() == 0)
                delete this;
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
            for (size_t i = 0; i < tmwait_slots_.size() && time_base_ < now; ++i) {
                Slot * s = slot_at(tmwait_slots_[i]);
                tmwait_slots_.pop_front();
                time_base_ += Duration::seconds(s->timeo);
                slot_free(tmwait_slots_[i]);
            }
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

        Bus::Slot * Bus::slot_alloc()
        {
            if (array_.size() < 0xffff) {
                Slot * s = &array_[next_id_++];
                while (!(*s == Slot())) {
                    s = &array_[next_id_++];
                }
                s->id = next_id_ - 1;
                s->flags = Slot::sfUsed;
                return s;
            } else if (!tmwait_slots_.empty()) {
                Slot * s = slot_at(tmwait_slots_.front());
                tmwait_slots_.pop_front();
                time_base_ += Duration::seconds(s->timeo);
                return s;
            } else {
                return NULL;
            }
        }

        void Bus::slot_free(
            boost::uint16_t id)
        {
            array_.reset(id);
        }

        void Bus::slot_free(
            Slot & slot)
        {
            if (slot.flags & Slot::sfSignal) {
                // TODO: remove from signal_slots_
            }
            Time expire = Time::now() + Duration::seconds(TIME_WAIT);
            slot.cell = NULL; //  timeo = 0
            if (tmwait_slots_.empty()) {
                time_base_ = time_tail_ = expire;
            } else {
                Slot * s = slot_at(tmwait_slots_.back());
                s->timeo = (expire - time_tail_).total_seconds();
                time_tail_ += Duration::seconds(slot.timeo);
            }
            tmwait_slots_.push_back(slot.id);
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
