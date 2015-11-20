// UdpSessionListener2.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSessionListener2.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        UdpSessionListener2::UdpSessionListener2(
            UdpManager & manager, 
            UdpTunnel & tunnel, 
            UdpEndpoint const & endpoint, 
            UdpSession * session)
            : UdpSession(tunnel)
            , umgr_(manager)
            , endpoint_(endpoint)
            , session_(session)
        {
            tunnel.set_endpoint(endpoint.endpoints[0].endp);
            tunnel.tid_ = tunnel.id();
        }

        UdpSessionListener2::~UdpSessionListener2()
        {
        }

        void UdpSessionListener2::on_msg(
            Message * msg)
        {
            pass_msg_to(msg, session_);
        }

    } // namespace client
} // namespace trip
