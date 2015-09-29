// UdpTunnel.h

#ifndef _TRIP_CLIENT_UDP_UDP_TUNNEL_H_
#define _TRIP_CLIENT_UDP_UDP_TUNNEL_H_

#include "trip/client/udp/UdpEndpoint.h"
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

            void set_endpoint(
                Endpoint const & ep);

        public:
            virtual void on_send(
                void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                void * head, 
                NetBuffer & buf);

            virtual void on_timer();

        protected:
            friend class UdpSessionListener;
            boost::uint16_t tid_; // id of remote tunnel point
            boost::uint16_t seq_;
            boost::asio::ip::udp::endpoint endpoint_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_TUNNEL_H_
