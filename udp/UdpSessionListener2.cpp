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
            UdpEndpoint const & remote, 
            UdpSession * session)
            : UdpSession(tunnel)
            , umgr_(manager)
            , session_(session)
        {
            UdpEndpoint const & local(umgr_.local_endpoint());   
            for (size_t i = 0; i < local.endpoints.size(); ++i) {
                Endpoint const & l = local.endpoints[i];
                if (l.type() != Endpoint::local) continue;
                for (size_t j = 0; j < remote.endpoints.size(); ++j) {
                    Endpoint const & r = remote.endpoints[j];
                    endpoints_.push_back(std::make_pair(l, r));
                }
            }
            tunnel.ep_pairs_ = &endpoints_;
            tunnel.p_id(tunnel.l_id());
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
