// UdpSocket.h

#ifndef _TRIP_P2P_UDP_SOCKET_H_
#define _TRIP_P2P_UDP_SOCKET_H_

#include "trip/client/net/Bus.h"

#include <framework/network/Endpoint.h>

#include <boost/asio/ip/udp.hpp>

namespace trip
{
    namespace client
    {

        class UdpTunnel;
        class UdpPacket;

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

        private:
            void handle_recv(
                UdpPacket * pkt, 
                boost::system::error_code ec, 
                size_t bytes_recved);

            void send();

        private:
            boost::asio::ip::udp::socket socket_;
            UdpTunnel * main_tul_;
            bool stopped_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_UDP_SOCKET_H_
