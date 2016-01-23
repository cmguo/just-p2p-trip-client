// SspBus.h

#ifndef _TRIP_SSP_SSP_BUS_H_
#define _TRIP_SSP_SSP_BUS_H_

#include "trip/client/net/Bus.h"

#include <framework/network/Endpoint.h>

#include <boost/asio/ip/udp.hpp>

namespace trip
{
    namespace client
    {

        class SspTunnel;
        class SspPacket;
        struct SspEndpoint;

        class SspBus
            : public Bus
        {
        public:
            SspBus(
                boost::asio::io_service & io_svc);

            ~SspBus();

            bool start(
                framework::network::Endpoint const & endpoint,
                boost::system::error_code & ec);

            bool stop(
                boost::system::error_code & ec);

            void handle_timer(
                Time const & now);

        public:
            boost::asio::io_service & io_svc()
            {
                return io_svc_;
            }

            void get_endpoint(
                SspEndpoint & endpoint);

        private:
            boost::asio::io_service & io_svc_;
            bool stopped_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_SSP_SSP_BUS_H_
