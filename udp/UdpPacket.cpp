// UdpPacket.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpPacket.h"

namespace trip
{
    namespace client
    {

        UdpPacket::UdpPacket()
            : sender(NULL)
            , sendctx(NULL)
        {
        }

        bool UdpPacket::decode()
        {
            return true;
        }

        bool UdpPacket::encode()
        {
            return true;
        }

    } // namespace client
} // namespace trip
