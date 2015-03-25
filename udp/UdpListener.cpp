// UdpListener.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpListener.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpPeer.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        UdpListener::UdpListener(
            UdpManager & manager)
            : UdpSession(manager.get_tunnel(Uuid()))
            , mgr_(manager)
        {
        }

        UdpListener::~UdpListener()
        {
        }

        void UdpListener::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Connect:
                {
                    MessageRequestConnect & req
                        = msg->as<MessageRequestConnect>();
                    UdpPeer peer;
                    peer.id = req.pid;
                    recent_ = &mgr_.get_tunnel(peer);
                }
                break;
            default:
                if (recent_)
                    pass_msg_to(msg, recent_->main_session());
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
