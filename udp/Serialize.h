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

        /*
        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpSession & t)
        {
            serialize(ar, (Cell &)t);
            ar & SERIALIZATION_NVP_NAME("l_seq", t.l_seq());
            ar & SERIALIZATION_NVP_NAME("p_seq", t.p_seq());
            ar & SERIALIZATION_NVP_NAME("ep_pairs", t.ep_pairs());
        }
        */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpTunnel & t)
        {
            //Bus::cell_visitor_t visitor = serialize_cell<Archive, UdpSession>(ar);
            //ar.context(&visitor);
            serialize(ar, (Bus &)t);
            //ar.context(NULL);
            ar & SERIALIZATION_NVP_2(t, l_seq);
            ar & SERIALIZATION_NVP_2(t, p_seq);
            if (t.ep_pairs())
                ar & SERIALIZATION_NVP_2(t, ep_pairs);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpSocket & t)
        {
            Bus::cell_visitor_t visitor = serialize_cell<Archive, UdpTunnel>(ar);
            ar.context(&visitor);
            serialize(ar, (Bus &)t);
            ar.context(NULL);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpManager & t)
        {
            ar 
                & SERIALIZATION_NVP_2(t, local_endpoint)
                & SERIALIZATION_NVP_2(t, socket)
                & SERIALIZATION_NVP_2(t, tunnels)
                & SERIALIZATION_NVP_2(t, stateless_tunnels)
                ;
        }

    }
}

#endif // _TRIP_CLIENT_UDP_SERIALIZE_H_
