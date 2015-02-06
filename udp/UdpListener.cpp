// UdpListener.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpListener.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        UdpListener::UdpListener(
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
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
//                    UdpSession *  tunnel = new UdpSession((UdpSession &)*bus_, req);
                }
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
