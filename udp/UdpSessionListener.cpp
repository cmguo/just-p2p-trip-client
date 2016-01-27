// UdpSessionListener.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSessionListener.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.h"
#include "trip/client/proto/Message.hpp"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpSessionListener", framework::logger::Debug);

        UdpSessionListener::UdpSessionListener(
            UdpManager & manager, 
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
            , umgr_(manager)
            , msg_try_(0)
            , status_(0)
            , seq_(0)
            , recent_(NULL)
        {
        }

        UdpSessionListener::~UdpSessionListener()
        {
        }

        void UdpSessionListener::set_remote(
            UdpEndpoint const & remote)
        {
            if (!endpoint_.endpoints.empty())
                return;
            endpoint_ = remote;
            UdpEndpoint const & local(umgr_.local_endpoint());   
            for (size_t i = 0; i < local.endpoints.size(); ++i) {
                Endpoint const & l = local.endpoints[i];
                if (l.type() != Endpoint::local) continue;
                for (size_t j = 0; j < remote.endpoints.size(); ++j) {
                    Endpoint const & r = remote.endpoints[j];
                    endpoints_.push_back(std::make_pair(l, r));
                }
            }
            tunnel().ep_pairs_ = &endpoints_;
            Time now;
            msg_time_ = now - Duration::seconds(1);
            on_timer(now);
        }

        /*
        bool UdpSessionListener::empty() const
        {
            return msg_sent_ == endpoint_.endpoints.size();
        }
        */

        /*
        void UdpSessionListener::on_send(
            //void * head, 
            NetBuffer & buf)
        {
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            pkt.endp = endpoint_.endpoints[msg_sent_];
            tunnel().set_endpoints(&endpoints_);
            ++msg_sent_;
            Message msg;
            MessageRequestConnect & req = 
                msg.get<MessageRequestConnect>();
            req.tid = tunnel().id();
            req.uid = umgr_.local_endpoint().id;
            MessageTraits::o_archive_t oa(buf);
            oa << msg;
            UdpSession::on_send(head, buf);
        }
        */

        void UdpSessionListener::on_recv(
            //void * head, 
            NetBuffer & buf)
        {
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            if (pkt.th.seq != seq_) {
                pkt_ep_ = pkt.endp;
                seq_ = pkt.th.seq;
                recent_ = NULL;
            }
            UdpSession::on_recv(/*head, */buf);
        }

        void UdpSessionListener::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Connect:
                {
                    MessageRequestConnect & req
                        = msg->as<MessageRequestConnect>();
                    set_remote(req.tid);
                    MessageResponseConnect resp;
                    resp.tid = tunnel().l_id();
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case REQ_Ping:
                {
                    MessageRequestPing & req
                        = msg->as<MessageRequestPing>();
                    MessageResponsePing resp;
                    resp.timestamp = req.timestamp;
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case REQ_Disconnect:
                {
                    MessageRequestDisconnect & req
                        = msg->as<MessageRequestDisconnect>();
                    (void)req;
                    MessageResponseDisconnect resp;
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case RSP_Connect:
                {
                    MessageResponseConnect & resp =
                        msg->as<MessageResponseConnect>();
                    set_remote(resp.tid);
                    free_message(msg);
                }
                break;
            case RSP_Ping:
                {
                    free_message(msg);
                }
                break;
            case RSP_Disconnect:
                {
                    free_message(msg);
                }
                break;
            default:
                if (recent_ == NULL)
                    recent_ = umgr_.get_session(tunnel(), *msg);
                else if (recent_)
                    pass_msg_to(msg, recent_);
                else
                    free_message(msg);
                break;
            }
        }

        void UdpSessionListener::on_timer(
            Time const & now)
        {
            if (now < msg_time_)
                return;
            if (status_ == 0) {
                if (msg_try_)
                    LOG_WARN("[on_timer] retry connect, ep:" << endpoint_.endpoints[0].to_string());
                ++msg_try_;
                msg_time_ = now + Duration::milliseconds(500);
                Message * msg = alloc_message();
                MessageRequestConnect & req = 
                    msg->get<MessageRequestConnect>();
                req.tid = tunnel().l_id();
                req.uid = umgr_.local_endpoint().id;
                send_msg(msg);
            } else if (now >= tunnel().stat_.recv_bytes().time + Duration::seconds(5)) {
                if (now >= tunnel().stat_.recv_bytes().time + Duration::seconds(60)) {
                    status_ = 0;
                    on_timer(now);
                    return;
                }
                msg_time_ = now + Duration::seconds(1);
                Message * msg = alloc_message();
                MessageRequestPing & req = 
                    msg->get<MessageRequestPing>();
                req.timestamp = framework::timer::ClockTime::tick_count();
                send_msg(msg);
            }

        }

        void UdpSessionListener::set_fake_sid(
            boost::uint16_t id)
        {
            UdpSession::p_id(id);
        }

        void UdpSessionListener::set_remote(
            boost::uint16_t id)
        {
            LOG_DEBUG("[set_remote], l_id: " << tunnel().l_id() << ", r_id: " << id);
            if (status_ == 0) {
                status_ = 1;
            }
            tunnel().p_id(id);
            endpoints_.clear();
            endpoints_.push_back(std::make_pair(pkt_ep_, pkt_ep_));
        }

    } // namespace client
} // namespace trip
