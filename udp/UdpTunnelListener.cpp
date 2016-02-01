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
            , seq_(0)
            , recent_(NULL)
        {
        }

        UdpTunnelListener::~UdpTunnelListener()
        {
        }

        void UdpTunnelListener::on_recv(
            NetBuffer & buf)
        {
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            if (pkt.th.seq != seq_) {
                seq_ = pkt.th.seq;
                recent_ = NULL;
            }
            UdpSession::on_recv(buf);
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
                    recent_ = &mgr_.get_tunnel(endp);
                }
            default:
                if (recent_) {
                    pass_msg_to(msg, recent_->main_session());
                } else {
                    LOG_DEBUG("[on_recv] unknown msg recv on main tunnel, type=" << msg_type_str(msg->type));
                }
                break;
            }
        }

    } // namespace client
} // namespace trip
