// UdpSocket.h

#ifndef _TRIP_UDP_UDP_SOCKET_H_
#define _TRIP_UDP_UDP_SOCKET_H_

#include "trip/client/net/Bus.h"

#include <framework/network/Endpoint.h>

#include <boost/asio/ip/udp.hpp>

namespace trip
{
    namespace client
    {

        class UdpTunnel;
        class UdpPacket;
        struct UdpEndpoint;

        class UdpSocket
            : public Bus
        {
        public:
            UdpSocket(
                boost::asio::io_service & io_svc);

            ~UdpSocket();

        public:
            bool start(
                framework::network::Endpoint const & endpoint,
                size_t parallel, 
                boost::system::error_code & ec);

            bool stop(
                boost::system::error_code & ec);

            void handle_timer(
                Time const & now);

        public:
            void get_endpoint(
                UdpEndpoint & endpoint);

        private:
            void handle_recv(
                UdpPacket * pkt, 
                boost::system::error_code ec, 
                size_t bytes_recved);

            void send();

        private:
            boost::asio::ip::udp::socket socket_;
            bool stopped_;
            UdpPacket * rcv_pkt_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_SOCKET_H_
