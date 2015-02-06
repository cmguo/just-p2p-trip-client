// UdpPacket.h

#ifndef _TRIP_P2P_UDP_PACKET_H_
#define _TRIP_P2P_UDP_PACKET_H_

#include "trip/client/proto/Packet.h"

#include <framework/memory/BigFixedPool.h>
#include <framework/memory/MemoryPoolObject.h>

#include <boost/asio/ip/udp.hpp>

namespace trip
{
    namespace client
    {

        struct UdpPacket
            : Packet
            , framework::memory::MemoryPoolObject<
                UdpPacket>
        {
            bool decode();

            bool encode();

            boost::asio::ip::udp::endpoint endp;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_UDP_PACKET_H_
