// UdpMainSession.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpMainSession.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        UdpMainSession::UdpMainSession(
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
        {
        }

        UdpMainSession::~UdpMainSession()
        {
        }

        void UdpMainSession::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Ping:
                {
                    MessageRequestPing & req
                        = msg->as<MessageRequestPing>();
                    MessageResponsePing resp;
                }
                break;
            case REQ_Disconnect:
                {
                    MessageRequestDisconnect & req
                        = msg->as<MessageRequestDisconnect>();
                    MessageResponseDisconnect resp;
                    inactive();
                }
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
