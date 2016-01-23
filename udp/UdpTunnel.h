// UdpTunnel.h

#ifndef _TRIP_CLIENT_UDP_UDP_TUNNEL_H_
#define _TRIP_CLIENT_UDP_UDP_TUNNEL_H_

#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/net/Bus.h"

namespace trip
{
    namespace client
    {

        class UdpSocket;
        class UdpSession;

        struct TunnelStat
        {
            boost::uint32_t bw; // band width in bps
            boost::uint32_t rtt; // in ms
            boost::uint32_t rtt2;
        };

        class UdpTunnel
            : public Bus
        {
        public:
            UdpTunnel(
                UdpSocket & socket);

            virtual ~UdpTunnel();

        public:
            UdpSession * main_session();

            boost::uint16_t l_seq() const
            {
                return l_seq_;
            }

            boost::uint16_t p_seq() const
            {
                return p_seq_;
            }

            UdpEndpointPairs const * ep_pairs() const
            {
                return ep_pairs_;
            }

        public:
            void set_endpoints(
                UdpEndpointPairs const * ep);

        public:
            virtual void on_send(
                //void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                //void * head, 
                NetBuffer & buf);

            virtual void on_timer(
                Time const & now);

        protected:
            friend class UdpSessionListener;
            friend class UdpSessionListener2;
            boost::uint16_t l_seq_;
            boost::uint16_t p_seq_;
            UdpEndpointPairs const * ep_pairs_;
            TunnelStat tunnel_stat_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_TUNNEL_H_
