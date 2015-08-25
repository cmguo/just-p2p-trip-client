// UdpPeer.h

#ifndef _TRIP_P2P_P2P_PEER_H_
#define _TRIP_P2P_P2P_PEER_H_

#include "trip/client/udp/UdpPeer.h"

namespace trip
{
    namespace client
    {

        struct P2pPeer
            : UdpPeer
        {
            Uuid id;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_P2P_PEER_H_
