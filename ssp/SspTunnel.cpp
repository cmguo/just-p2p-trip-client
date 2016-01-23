// SspTunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/ssp/SspTunnel.h"
#include "trip/client/ssp/SspBus.h"
#include "trip/client/ssp/SspSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/net/Queue.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#define SHARER_QUEUE_SIZE 16

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.SspTunnel", framework::logger::Debug);

        SspTunnel::SspTunnel(
            SspBus & bus)
            : Bus(&bus)
            , ep_pairs_(NULL)
        {
        }

        SspTunnel::~SspTunnel()
        {
        }

        boost::asio::io_service & SspTunnel::io_svc()
        {
            return static_cast<SspBus *>(bus_)->io_svc();
        }

        SspSession * SspTunnel::main_session()
        {
            return static_cast<SspSession *>(slot_at(uint16_t(0))->cell);
        }

        void SspTunnel::on_send(
            NetBuffer & buf)
        {
            Bus::on_send(buf);
            bus_->on_send(buf);
        }

        void SspTunnel::on_recv(
            NetBuffer & buf)
        {
            bus_->on_recv(buf); // push to SspBus for sum
            Bus::on_recv(buf);
        }

        /*
        void SspTunnel::on_timer(
            Time const & now)
        {
            Bus::on_timer(now);
        }
        */

    } // namespace client
} // namespace trip
