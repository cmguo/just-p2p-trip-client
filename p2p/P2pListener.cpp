// P2pListener.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pListener.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        P2pListener::P2pListener(
            UdpTunnel & tunnel)
            : UdpMainSession(tunnel)
        {
        }

        P2pListener::~P2pListener()
        {
        }

        void P2pListener::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Bind:
                {
                    MessageRequestBind & req
                        = msg->as<MessageRequestBind>();
//                    P2pSink * sink = new P2pSink((UdpTunnel &)*bus_, req);
                }
                break;
            default:
                UdpMainSession::on_msg(msg);
            }
        }

    } // namespace client
} // namespace trip
