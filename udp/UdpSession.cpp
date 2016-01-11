// UdpSession.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageTunnel.h"
#include "trip/client/proto/Message.hpp"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpSession", framework::logger::Debug);

        UdpSession::UdpSession()
            : sid_(0)
        {
        }

        UdpSession::UdpSession(
            UdpTunnel & tunnel)
            : Cell(&tunnel)
            , sid_(0)
        {
        }

        UdpSession::~UdpSession()
        {
        }

        UdpTunnel & UdpSession::tunnel()
        {
            return *(UdpTunnel *)bus_;
        }

        void UdpSession::on_send(
            //void * head, 
            NetBuffer & buf)
        {
            Cell::on_send(/*head, */buf);
        }

        void UdpSession::on_recv(
            //void * head, 
            NetBuffer & buf)
        {
            Cell::on_recv(/*head, */buf);
            MessageTraits::i_archive_t ar(buf);
            Message * msg = alloc_message();
            ar >> *msg;
            //LOG_DEBUG("[on_msg] type=" << msg_type_str(msg->type));
            on_msg(msg);
        }

        bool UdpSession::send_msg(
            Message * msg)
        {
            //LOG_DEBUG("[send_msg] type=" << msg_type_str(msg->type));
            msg->sid = sid_;
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

    } // namespace client
} // namespace trip
