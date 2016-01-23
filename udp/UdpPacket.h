// UdpPacket.h

#ifndef _TRIP_UDP_UDP_PACKET_H_
#define _TRIP_UDP_UDP_PACKET_H_

#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/proto/Packet.h"
#include "trip/client/proto/TunnelHeader.h"

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
            UdpPacket();

            bool decode();

            bool encode();

            boost::asio::ip::udp::endpoint endp;
            UdpEndpointPairs const * endpairs;
            TunnelHeader th;
            //void (*sender)(
            //    boost::asio::ip::udp::socket & socket, 
            //    UdpPacket & packet, 
            //    void * context);
            //void * sendctx;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_PACKET_H_
