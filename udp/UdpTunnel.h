// UdpTunnel.h

#ifndef _TRIP_CLIENT_UDP_UDP_TUNNEL_H_
#define _TRIP_CLIENT_UDP_UDP_TUNNEL_H_

#include "trip/client/net/Tunnel.h"

namespace trip
{
    namespace client
    {

        class UdpSocket;
        class UdpSession;

        class UdpTunnel
            : public Tunnel
        {
        public:
            UdpTunnel(
                UdpSocket & socket);

            virtual ~UdpTunnel();

        public:
            UdpSession * main_session();

        public:
            virtual void on_send(
                void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                void * head, 
                NetBuffer & buf);

            virtual void on_timer();

        protected:
            boost::uint16_t peer_id_;
            boost::uint16_t seq_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_TUNNEL_H_
