// SspBus.cpp

#include "trip/client/Common.h"
#include "trip/client/ssp/SspBus.h"
#include "trip/client/timer/TimerManager.h"

#include <util/daemon/Daemon.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.SspBus", framework::logger::Debug);

        SspBus::SspBus(
            boost::asio::io_service & io_svc)
            : io_svc_(io_svc)
            , stopped_(false)
        {
        }

        SspBus::~SspBus()
        {
        }

        bool SspBus::start(
            framework::network::Endpoint const & endpoint, 
            boost::system::error_code & ec)
        {
            return true;
        }

        bool SspBus::stop(
            boost::system::error_code & ec)
        {
            stopped_ = true;
            return !ec;
        }

        void SspBus::handle_timer(
            Time const & now)
        {
            on_timer(now);
        }

    } // namespace client
} // namespace trip
