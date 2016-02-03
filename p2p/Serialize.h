// Serialize.h

#ifndef _TRIP_CLIENT_P2P_SERIALIZE_H_
#define _TRIP_CLIENT_P2P_SERIALIZE_H_

#include "trip/client/p2p/P2pManager.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pSink.h"
#include "trip/client/udp/Serialize.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/net/Serialize.h"

#include <util/serialization/stl/vector.h>
#include <util/serialization/stl/map.h>
#include <util/serialization/Url.h>
#include <util/serialization/Uuid.h>

namespace trip
{
    namespace client
    {

        /* P2pSource */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            P2pSource::Request & t)
        {
            ar & SERIALIZATION_NVP_1(t, id)
                & SERIALIZATION_NVP_1(t, time);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            P2pSource & t)
        {
            serialize(ar, (UdpSession &)t);
            ar & SERIALIZATION_NVP_3(t, map_id)
                & SERIALIZATION_NVP_3(t, map)
                & SERIALIZATION_NVP_3(t, delta)
                & SERIALIZATION_NVP_3(t, rtt)
                & SERIALIZATION_NVP_3(t, requests)
                & SERIALIZATION_NVP_3(t, req_count)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            P2pSink & t)
        {
            serialize(ar, (UdpSession &)t);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            P2pManager & t)
        {
            ar & SERIALIZATION_NVP_2(t, sources);
            ar & SERIALIZATION_NVP_2(t, sinks);
        }

    }
}

#endif // _TRIP_CLIENT_P2P_SERIALIZE_H_
