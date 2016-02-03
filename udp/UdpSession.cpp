// UdpSession.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.hpp"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#if 1
#  undef LOG_DEBUG
#  define LOG_DEBUG(xxx)
#endif

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpSession", framework::logger::Debug);

        UdpSession::UdpSession()
        {
        }

        UdpSession::UdpSession(
            UdpTunnel & tunnel)
            : Cell(&tunnel)
        {
        }

        UdpSession::~UdpSession()
        {
        }

        UdpTunnel & UdpSession::tunnel()
        {
            return *(UdpTunnel *)bus_;
        }

        bool UdpSession::is_open() const
        {
            return p_id() != 0;
        }

        void UdpSession::on_send(
            NetBuffer & buf)
        {
            Cell::on_send(buf);
        }

        void UdpSession::on_recv(
            NetBuffer & buf)
        {
            Cell::on_recv(buf);
            MessageTraits::i_archive_t ar(buf);
            Message * msg = alloc_message();
            ar >> *msg;
            LOG_DEBUG("[on_recv] "
                << p_id() << " -> " << l_id()
                << " size=" << msg->size
                << ", type=" << msg_type_str(msg->type));
            on_msg(msg);
        }

        bool UdpSession::send_msg(
            Message * msg)
        {
            //LOG_DEBUG("[send_msg] type=" << msg_type_str(msg->type));
            msg->sid = p_id();
            return push(msg);
        }

        void UdpSession::pass_msg_to(
            Message * msg, 
            UdpSession * to)
        {
            to->on_msg(msg);
        }

        void UdpSession::on_msg(
            Message * msg)
        {
        }

        void UdpSession::on_tunnel_connecting()
        {
        }

        void UdpSession::on_tunnel_disconnect()
        {
            p_id(0);
        }

    } // namespace client
} // namespace trip
