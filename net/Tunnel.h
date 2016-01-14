// Tunnel.h

#ifndef _TRIP_CLIENT_NET_TUNNEL_H_
#define _TRIP_CLIENT_NET_TUNNEL_H_

#include "trip/client/net/Bus.h"

namespace trip
{
    namespace client
    {

        struct TunnelStat
        {
            boost::uint32_t bw; // band width in bps
            boost::uint32_t rtt; // in ms
            boost::uint32_t rtt2;
        };

        class Tunnel
            : public Bus
        {
        public:
            Tunnel(
                Bus * bus = NULL);

            virtual ~Tunnel();

        public:
            using Bus::on_send;
            using Bus::on_recv;

            virtual void on_send(
                //void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                //void * head, 
                NetBuffer & buf);

        protected:
            TunnelStat tunnel_stat_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_TUNNEL_H_
