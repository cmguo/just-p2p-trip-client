// UdpPeer.h

#ifndef _TRIP_P2P_UDP_PEER_H_
#define _TRIP_P2P_UDP_PEER_H_

#include <framework/network/Endpoint.h>

namespace trip
{
    namespace client
    {

        using framework::network::Endpoint;

        struct UdpPeer
        {
            Uuid id;
            boost::uint16_t num_ep;
            Endpoint endpoints[4];

            UdpPeer()
                : num_ep(0)
            {
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_UDP_PEER_H_
