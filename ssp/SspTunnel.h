// SspTunnel.h

#ifndef _TRIP_CLIENT_SSP_SSP_TUNNEL_H_
#define _TRIP_CLIENT_SSP_SSP_TUNNEL_H_

#include "trip/client/ssp/SspEndpoint.h"
#include "trip/client/net/Bus.h"

namespace trip
{
    namespace client
    {

        class SspBus;
        class SspSession;

        struct SspTunnelStat
        {
            boost::uint32_t bw; // band width in bps
            boost::uint32_t rtt; // in ms
            boost::uint32_t rtt2;
        };

        class SspTunnel
            : public Bus
        {
        public:
            SspTunnel(
                SspBus & socket, 
                SspEndpoint const & endp);

            virtual ~SspTunnel();

        public:
            boost::asio::io_service & io_svc();

            SspEndpoint const & endpoint() const;

            Uuid const & pid() const;

        public:
            void set_endpoints(
                SspEndpointPairs const * ep);

        public:
            virtual void on_send(
                //void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                //void * head, 
                NetBuffer & buf);

            /*
            virtual void on_timer(
                Time const & now);
            */

        protected:
            SspEndpoint endp_;
            SspTunnelStat tunnel_stat_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_SSP_SSP_TUNNEL_H_
