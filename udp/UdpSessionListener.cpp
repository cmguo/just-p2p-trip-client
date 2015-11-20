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
                status_ = 1; // no need probe & connect
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
            MessageRequestConnect & req = 
                msg.get<MessageRequestConnect>();
            req.tid = tunnel().id();
            req.uid = umgr_.local_endpoint().id;
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
            case REQ_Connect:
                {
                    MessageRequestConnect & req
                        = msg->as<MessageRequestConnect>();
                    set_remote(req.tid);
                    MessageResponseConnect resp;
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
                }
            }
        }

        void UdpSessionListener::set_remote(
            boost::uint16_t id)
        {
            if (status_ == 0) {
                status_ = 1;
                tunnel().set_endpoint(pkt_ep_);
                tunnel().tid_ = id;
            } else if (tunnel().tid_ != id) {
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
