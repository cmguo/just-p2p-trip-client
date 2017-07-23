// Serialize.h

#ifndef _TRIP_CLIENT_NET_SERIALIZE_H_
#define _TRIP_CLIENT_NET_SERIALIZE_H_

#include "trip/client/net/Cell.h"
#include "trip/client/net/Bus.h"
#include "trip/client/net/Queue.h"

#include <util/serialization/stl/vector.h>
#include <util/serialization/stl/deque.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            StatValue & t)
        {
            ar 
                & SERIALIZATION_NVP_1(t, total)
                & SERIALIZATION_NVP_1(t, last)
                & SERIALIZATION_NVP_1(t, speed)
                & SERIALIZATION_NVP_1(t, speed_max)
                & SERIALIZATION_NVP_1(t, speed_avg)
                & SERIALIZATION_NVP_1(t, time)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Stat & t)
        {
            ar 
                & SERIALIZATION_NVP_2(t, time_next)
                & SERIALIZATION_NVP_2(t, time_last)
                & SERIALIZATION_NVP_2(t, send_bytes)
                & SERIALIZATION_NVP_2(t, recv_bytes)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Queue & t)
        {
            ar 
                & SERIALIZATION_NVP_2(t, empty)
                & SERIALIZATION_NVP_2(t, size)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Cell & t)
        {
            ar 
                & SERIALIZATION_NVP_NAME("this", (intptr_t)&t)
                & SERIALIZATION_NVP_2(t, l_id)
                & SERIALIZATION_NVP_2(t, p_id)
                & SERIALIZATION_NVP_2(t, empty)
                & SERIALIZATION_NVP_2(t, queue)
                & SERIALIZATION_NVP_2(t, queue)
                ;
        }

        template <typename Archive, typename T = Cell>
        struct serialize_cell
        {
            serialize_cell(
                Archive & ar)
                : ar_(ar)
            {
            }
            void operator()(
                Cell & t) const
            {
                ar_ & SERIALIZATION_NVP_NAME("item", static_cast<T &>(t));
            }
            Archive & ar_;
        };

        struct BusCells
        {
            BusCells(Bus & bus)
                : bus_(bus)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP_NAME("count", bus_.count());
                if (ar.context()) {
                    Bus::cell_visitor_t * visitor = (Bus::cell_visitor_t *)ar.context();
                    ar.context(NULL);
                    bus_.foreach_cell(*visitor);
                    ar.context(visitor);
                } else {
                    bus_.foreach_cell(serialize_cell<Archive>(ar));
                }
            }

            Bus & bus_;
        };

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Bus & t)
        {
            serialize(ar, (Cell &)t);
            ar 
                & SERIALIZATION_NVP_NAME("cells", BusCells(t))
                & SERIALIZATION_NVP_2(t, signal_slots)
                & SERIALIZATION_NVP_2(t, tmwait_slots)
                ;
        }

    }
}

#endif // _TRIP_CLIENT_NET_SERIALIZE_H_
