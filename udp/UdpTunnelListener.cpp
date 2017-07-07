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

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpTunnelListener", framework::logger::Debug);

        UdpTunnelListener::UdpTunnelListener(
            UdpManager & manager, 
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
            , mgr_(manager)
        {
        }

        UdpTunnelListener::~UdpTunnelListener()
        {
        }

        void UdpTunnelListener::on_recv(
            NetBuffer & buf)
        {
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            pkt_ = &pkt;
            if (pkt.recent1 == NULL) {
                size_t pos = buf.pubseekoff(0, std::ios::cur, std::ios::in);
                UdpSession::on_recv(buf);
                buf.pubseekoff(pos, std::ios::beg, std::ios::in);
            }
            if (pkt.recent1)
                pkt.recent1->main_session()->on_recv(buf);
        }

        void UdpTunnelListener::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Connect:
                {
                    MessageRequestConnect & req
                        = msg->as<MessageRequestConnect>();
                    if (req.uid == mgr_.local_endpoint().id)
                        break;
                    UdpEndpoint endp;
                    endp.id = req.uid;
                    pkt_->recent1 = &mgr_.get_tunnel(endp);
                }
                break;
            default:
                LOG_DEBUG("[on_recv] unknown msg recv on main tunnel, type=" << msg_type_str(msg->type));
                break;
            }
            free_message(msg);
        }

    } // namespace client
} // namespace trip
