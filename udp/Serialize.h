// Serialize.h

#ifndef _TRIP_CLIENT_UDP_SERIALIZE_H_
#define _TRIP_CLIENT_UDP_SERIALIZE_H_

#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpSession.h"
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

        using util::serialization::make_sized;

        /* UdpEndpoint */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpEndpoint & endpoint)
        {
            ar & SERIALIZATION_NVP_NAME("id", endpoint.id);
            ar & SERIALIZATION_NVP_NAME("endpoints", make_sized<boost::uint32_t>(endpoint.endpoints));
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpSocket & t)
        {
            serialize(ar, (Bus &)t);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpTunnel & t)
        {
            ar & SERIALIZATION_NVP_NAME("l_id", t.l_id());
            ar & SERIALIZATION_NVP_NAME("p_id", t.p_id());
            ar & SERIALIZATION_NVP_NAME("l_seq", t.l_seq());
            ar & SERIALIZATION_NVP_NAME("p_seq", t.p_seq());
            ar & SERIALIZATION_NVP_NAME("ep_pairs", t.ep_pairs());
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpManager & t)
        {
            ar & SERIALIZATION_NVP_NAME("local_endpoint", t.local_endpoint());
            ar & SERIALIZATION_NVP_NAME("socket", t.socket());
            ar & SERIALIZATION_NVP_NAME("tunnels", t.tunnels());
            ar & SERIALIZATION_NVP_NAME("pulse_tunnels", t.pulse_tunnels());
        }

    }
}

#endif // _TRIP_CLIENT_UDP_SERIALIZE_H_
