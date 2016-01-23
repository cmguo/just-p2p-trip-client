// SspSession.cpp

#include "trip/client/Common.h"
#include "trip/client/ssp/SspSession.h"
#include "trip/client/ssp/SspTunnel.h"
#include "trip/client/proto/Message.hpp"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.SspSession", framework::logger::Debug);

        SspSession::SspSession()
        {
        }

        SspSession::SspSession(
            SspTunnel & tunnel)
            : Cell(&tunnel)
        {
        }

        SspSession::~SspSession()
        {
        }

        SspTunnel & SspSession::tunnel()
        {
            return *(SspTunnel *)bus_;
        }

        void SspSession::on_send(
            NetBuffer & buf)
        {
            Cell::on_send(buf);
            bus_->on_send(buf);
        }

        void SspSession::on_recv(
            NetBuffer & buf)
        {
            bus_->on_recv(buf);
            Cell::on_recv(buf);
        }

        /*
        bool SspSession::send_msg(
            Message * msg)
        {
            //LOG_DEBUG("[send_msg] type=" << msg_type_str(msg->type));
            msg->sid = p_id();
            return push(msg);
        }

        void SspSession::on_msg(
            Message * msg)
        {
        }
        */

    } // namespace client
} // namespace trip
