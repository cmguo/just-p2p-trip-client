// Serialize.h

#ifndef _TRIP_CLIENT_NET_SERIALIZE_H_
#define _TRIP_CLIENT_NET_SERIALIZE_H_

#include "trip/client/net/Cell.h"
#include "trip/client/net/Bus.h"

#include <util/serialization/stl/vector.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            StatValue & t)
        {
            ar & SERIALIZATION_NVP_1(t, total)
                & SERIALIZATION_NVP_1(t, last)
                & SERIALIZATION_NVP_1(t, speed)
                & SERIALIZATION_NVP_1(t, speed_max)
                & SERIALIZATION_NVP_1(t, speed_avg);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Stat & t)
        {
            ar & SERIALIZATION_NVP_NAME("send_bytes", t.send_bytes())
                & SERIALIZATION_NVP_NAME("recv_bytes", t.recv_bytes());
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Cell & t)
        {
            ar & SERIALIZATION_NVP_NAME("l_id", t.l_id());
            ar & SERIALIZATION_NVP_NAME("p_id", t.p_id());
            ar & SERIALIZATION_NVP_NAME("stat", t.stat());
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Bus & t)
        {
            serialize(ar, (Cell &)t);
        }

    }
}

#endif // _TRIP_CLIENT_NET_SERIALIZE_H_
