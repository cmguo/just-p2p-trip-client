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
            th.chk = checksum();
            if (th.chk == 0)
                return true;
            th.chk = 0;
            return false;
        }
        
        bool UdpPacket::encode()
        {
            th.chk = checksum();
            ((TunnelHeader *)head_)->chk = th.chk;
            return true;
        }

    } // namespace client
} // namespace trip
