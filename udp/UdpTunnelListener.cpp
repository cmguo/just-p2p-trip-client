// UdpTunnelListener.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpTunnelListener.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        UdpTunnelListener::UdpTunnelListener(
            UdpManager & manager, 
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
            , mgr_(manager)
            , seq_(0)
            , recent_(NULL)
        {
        }

        UdpTunnelListener::~UdpTunnelListener()
        {
        }

        void UdpTunnelListener::on_recv(
            void * head, 
            NetBuffer & buf)
        {
            UdpPacket * pkt = (UdpPacket *)head;
            if (pkt->th.seq != seq_) {
                seq_ = pkt->th.seq;
                recent_ = NULL;
            }
            size_t pos = buf.pubseekoff(0, std::ios::cur, std::ios::in);
            UdpSession::on_recv(head, buf);
            if (recent_) {
                pos = buf.pubseekoff(pos, std::ios::beg, std::ios::in);
                recent_->main_session()->on_recv(head, buf);
            }
        }

        void UdpTunnelListener::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Connect:
                {
                    MessageRequestConnect & req
                        = msg->as<MessageRequestConnect>();
                    UdpEndpoint endp;
                    endp.id = req.uid;
                    recent_ = &mgr_.get_tunnel(endp);
                }
                break;
            default:
                break;
            }
        }

    } // namespace client
} // namespace trip
