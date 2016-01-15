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
            , signal_slots_(NULL)
            , signal_slots_tail_(&signal_slots_)
            , tmwait_slots_(NULL)
            , tmwait_slots_tail_(&tmwait_slots_)
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
            slot.id = cell->l_id();
            slot_free(slot);
            return true;
        }

        bool Bus::empty() const
        {
            return Cell::empty() && signal_slots_ == NULL;
        }

        void Bus::on_send(
            boost::uint16_t id, 
            //void * head, 
            NetBuffer & buf)
        {
            Slot * slot(slot_at(id));
            assert(slot);
            slot->cell->on_send(/*head, */buf);
        }

        void Bus::on_recv(
            boost::uint16_t id, 
            //void * head, 
            NetBuffer & buf)
        {
            Slot * slot(slot_at(id));
            assert(slot);
            slot->cell->on_recv(/*head, */buf);
        }

        void Bus::signal(
            Cell * cell)
        {
            Slot & slot(slot_at(cell));
            bool s = signal_slots_ == NULL;
            if ((slot.flags & Slot::sfSignal) == 0) {
                slot.flags |= Slot::sfSignal;
                *signal_slots_tail_ = &slot;
                signal_slots_tail_ = &slot.next;
            }
            if (s)
                signal();
        }

        void Bus::on_send(
            //void * head, 
            NetBuffer & buf)
        {
            if (signal_slots_) {
                Slot * slot = signal_slots_;
                signal_slots_ = signal_slots_->next;
                if (signal_slots_ == NULL)
                    signal_slots_tail_ = &signal_slots_;
                slot->cell->on_send(/*head, */buf);
                if (!slot->cell->empty()) {
                    *signal_slots_tail_ = slot;
                    signal_slots_tail_ = &slot->next;
                } else {
                    slot->flags &= ~Slot::sfSignal;
                }
            } else if (queue_) {

            }
            Cell::on_send(buf);
        }

        void Bus::on_recv(
            //void * head, 
            NetBuffer & buf)
        {
            Cell::on_recv(buf);
        }

        bool Bus::is_signal() const
        {
            return signal_slots_ != NULL;
        }

        void Bus::on_timer(
            Time const & now)
        {
            Cell::on_timer(now);
            if (root_.node == NULL)
                return;
            for (size_t i2 = 0; i2 < 32; ++i2) {
                Slot & slot2 = root_.node->slots[i2];
                if (slot2.node == NULL)
                    continue;
                for (size_t i1 = 0; i1 < 32; ++i1) {
                    Slot & slot1 = slot2.node->slots[i1];
                    if (slot1.node == NULL)
                        continue;
                    for (size_t i0 = 0; i0 < 32; ++i0) {
                        Slot & slot0 = slot1.node->slots[i0];
                        if (slot0.flags & Slot::sfActive)
                            slot0.cell->on_timer(now);
                    }
                }
            }
            while (tmwait_slots_ && time_base_ < now) {
                Slot * s = tmwait_slots_;
                tmwait_slots_ = tmwait_slots_->next;
                s->next = NULL;
                slot_free(s->id);
                if (tmwait_slots_) {
                    time_base_ += Duration::seconds(tmwait_slots_->timeo);
                    tmwait_slots_->timeo = 0;
                }
            }
        }

        Bus::Slot & Bus::slot_at(
            Cell * cell)
        {
            Slot * slot(slot_at(cell->l_id()));
            assert(slot->cell == cell);
            return *slot;
        }

        union Id
        {
            struct {
                boost::uint16_t i0 : 5;
                boost::uint16_t i1 : 5;
                boost::uint16_t i2 : 5;
                boost::uint16_t i : 1;
            };
            boost::uint16_t n;
            Id(boost::uint16_t id) { n = id; }
        };

        Bus::Slot * Bus::slot_at(
            boost::uint16_t id)
        {
            Id i(id);
            if (root_.node) {
                Slot & slot2 = root_.node->slots[i.i2];
                if (slot2.node) {
                    Slot & slot1 = slot2.node->slots[i.i1];
                    if (slot1.node) {
                        Slot & slot0 = slot1.node->slots[i.i0];
                        if (slot0.flags & Slot::sfActive)
                            return &slot0;
                    }
                }
            }
            return NULL;
        }

        Bus::Slot * Bus::slot_alloc()
        {
            Id i = next_id_;
            if (root_.node == NULL) {
                root_.node = new Node;
            }
            for (; i.i2 < 32; ++i.i2) {
                Slot & slot2 = root_.node->slots[i.i2];
                if (slot2.node == NULL) {
                    slot2.node = new Node;
                    ++root_.flags;
                }
                for (; i.i1 < 32; ++i.i1) {
                    Slot & slot1 = slot2.node->slots[i.i1];
                    if (slot1.node == NULL) {
                        slot1.node = new Node;
                        ++slot2.flags;
                    }
                    for (; i.i0 < 32; ++i.i0) {
                        Slot & slot0 = slot1.node->slots[i.i0];
                        if (slot0.flags == 0) {
                            slot0.flags = Slot::sfUsed;
                            slot0.id = i.n;
                            next_id_ = i.n + 1;
                            return &slot0;
                        }
                    }
                }
            }
            if (tmwait_slots_) {
                Slot * s = tmwait_slots_;
                tmwait_slots_ = s->next;
                if (tmwait_slots_) {
                    time_base_ += Duration::seconds(tmwait_slots_->timeo);
                    tmwait_slots_->timeo = 0;
                }
                return s;
            }
            return NULL;
        }

        void Bus::slot_free(
            boost::uint16_t id)
        {
            Id i(id);
            Slot & slot2 = root_.node->slots[i.i2];
            Slot & slot1 = slot2.node->slots[i.i1];
            Slot & slot0 = slot1.node->slots[i.i0];
            slot0.flags = 0;
            if (--slot1.flags == 0) {
                delete slot1.node;
                slot1.node = NULL;
                if (--slot2.flags == 0) {
                    delete slot2.node;
                    slot2.node = NULL;
                    if (--root_.flags == 0) {
                        delete root_.node;
                        root_.node = NULL;
                    }
                }
            }
        }

        void Bus::slot_free(
            Slot & slot)
        {
            Time expire = Time::now() + Duration::seconds(TIME_WAIT);
            slot.mark = 1;
            if (tmwait_slots_) {
                slot.timeo = (expire - time_tail_).total_seconds();
                time_tail_ += Duration::seconds(slot.timeo);
            } else {
                slot.timeo = 0;
                time_base_ = time_tail_ = expire;
            }
            *tmwait_slots_tail_ = &slot;
            tmwait_slots_tail_ = &slot.next;
        }

    } // namespace client
} // namespace trip
