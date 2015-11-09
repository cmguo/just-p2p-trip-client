// UdpSessionListener.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSessionListener.h"
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

        UdpSessionListener::UdpSessionListener(
            UdpManager & manager, 
            UdpTunnel & tunnel, 
            UdpEndpoint const & endpoint)
            : UdpSession(tunnel)
            , umgr_(manager)
            , endpoint_(endpoint)
            , msg_try_(0)
            , msg_sent_(0)
            , msg_recv_(0)
            , status_(0)
            , seq_(0)
            , recent_(NULL)
        {
            if (endpoint_.endpoints.empty())
                status_ = 2; // no need probe & connect
        }

        UdpSessionListener::~UdpSessionListener()
        {
        }

        bool UdpSessionListener::empty() const
        {
            return msg_sent_ < endpoint_.endpoints.size();
        }

        void UdpSessionListener::on_send(
            void * head, 
            NetBuffer & buf)
        {
            UdpPacket * pkt = (UdpPacket *)head;
            pkt->endp = endpoint_.endpoints[msg_sent_].endp;
            Message msg;
            MessageRequestProbe & req = 
                msg.get<MessageRequestProbe>();
            req.index = msg_sent_++;
            probe_rand_[req.index] = req.rand;
            MessageTraits::o_archive_t oa(buf);
            oa << msg;
        }

        void UdpSessionListener::on_recv(
            void * head, 
            NetBuffer & buf)
        {
            UdpPacket * pkt = (UdpPacket *)head;
            if (pkt->th.seq != seq_) {
                pkt_ep_ = pkt->endp;
                seq_ = pkt->th.seq;
                recent_ = NULL;
            }
            UdpSession::on_recv(head, buf);
        }

        void UdpSessionListener::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Probe:
                {
                    MessageRequestProbe & req
                        = msg->as<MessageRequestProbe>();
                    MessageResponseProbe resp;
                    resp.index = req.index;
                    resp.rand = req.rand;
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case REQ_Connect:
                {
                    tunnel().set_endpoint(pkt_ep_);
                    MessageRequestConnect & req
                        = msg->as<MessageRequestConnect>();
                    tunnel().tid_ = req.tid;
                    MessageResponseConnect resp;
                    resp.tid = tunnel().id();
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case REQ_Ping:
                {
                    MessageRequestPing & req
                        = msg->as<MessageRequestPing>();
                    MessageResponsePing resp;
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case REQ_Disconnect:
                {
                    MessageRequestDisconnect & req
                        = msg->as<MessageRequestDisconnect>();
                    MessageResponseDisconnect resp;
                    msg->reset(resp);
                    send_msg(msg);
                }
                break;
            case RSP_Probe:
                {
                    MessageResponseProbe & resp =
                        msg->as<MessageResponseProbe>();
                    if (resp.index < msg_sent_ && resp.rand == probe_rand_[resp.index]) {
                        msg_recv_ |= (1 << resp.index);
                    }
                    free_message(msg);
                }
                break;
            case RSP_Connect:
                {
                    MessageResponseConnect & resp =
                        msg->as<MessageResponseConnect>();
                    tunnel().tid_ = resp.tid;
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
            if (msg_sent_ == 0)
                return;
            if (now < msg_time_ + Duration::milliseconds(100))
                return;
            if (status_ == 0) {
                if (msg_recv_ == 0) {
                    msg_sent_ = 0;
                    ++msg_try_;
                    msg_time_ = now;
                    signal();
                } else {
                    for (boost::uint16_t i = 0; i < msg_sent_; ++i) {
                        if (msg_recv_ & (1 << i)) {
                            tunnel().set_endpoint(endpoint_.endpoints[i].endp);
                            break;
                        }
                    }
                    msg_try_ = 0;
                    msg_sent_ = 0;
                    msg_recv_ = 0;
                    status_ = 1;
                }
            }
            if (status_ == 1) {
                if (msg_recv_ == 0) {
                    Message * msg = alloc_message();
                    MessageRequestConnect & req = 
                        msg->get<MessageRequestConnect>();
                    req.tid = id();
                    send_msg(msg);
                    msg_sent_ = 1;
                    ++msg_try_;
                    msg_time_ = now;
                } else {
                    status_ = 2;
                }
            }
        }

    } // namespace client
} // namespace trip
