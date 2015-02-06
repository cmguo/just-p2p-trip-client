// UdpSession.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/Message.hpp"

namespace trip
{
    namespace client
    {

        UdpSession::UdpSession(
            UdpTunnel & tunnel)
            : Cell(&tunnel)
            , peer_id_(0)
        {
        }

        UdpSession::~UdpSession()
        {
        }

        void UdpSession::on_send(
            void * head, 
            NetBuffer & buf)
        {
            Message * msg = NULL;
            MessageTraits::o_archive_t ar(buf);
            while ((msg = (Message *)first())) {
                ar << *msg;
                pop();
                delete msg;
            }
            Cell::on_send(head, buf);
        }

        void UdpSession::on_recv(
            void * head, 
            NetBuffer & buf)
        {
            Cell::on_recv(head, buf);
            MessageTraits::i_archive_t ar(buf);
            while (buf.in_avail()) {
                Message * msg = new Message;
                ar >> *msg;
                on_msg(msg);
            }
        }

        bool UdpSession::send_msg(
            Message * msg)
        {
            msg->sid = peer_id_;
            return push(msg);
        }

    } // namespace client
} // namespace trip
