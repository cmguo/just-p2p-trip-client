// UdpSocket.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/PacketBuffers.h"
#include "trip/client/proto/TunnelHeader.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        UdpSocket::UdpSocket(
            boost::asio::io_service & io_svc)
            : socket_(io_svc)
            , stopped_(false)
        {
        }

        UdpSocket::~UdpSocket()
        {
        }

        bool UdpSocket::start(
            framework::network::Endpoint const & endpoint, 
            size_t parallel, 
            boost::system::error_code & ec)
        {
            boost::asio::ip::udp::endpoint ep(endpoint);
            socket_.open(ep.protocol(), ec);
            if (ec)
                return false;
            socket_.bind(ep, ec);
            if (ec)
                return false;
            for (size_t i = 0; i < parallel; ++i) {
                UdpPacket * pkt = new UdpPacket;
                socket_.async_receive(*pkt, 
                    boost::bind(&UdpSocket::handle_recv, this, pkt, _1, _2));
            }
            return true;
        }

        bool UdpSocket::stop(
            boost::system::error_code & ec)
        {
            stopped_ = true;
            socket_.close(ec);
            return !ec;
        }

        void UdpSocket::handle_recv(
            UdpPacket * pkt, 
            boost::system::error_code ec, 
            size_t bytes_recved)
        {
            if (ec) {
                if (stopped_) {
                    delete pkt;
                    return;
                }
                socket_.async_receive(*pkt, 
                    boost::bind(&UdpSocket::handle_recv, this, pkt, _1, _2));
                return;
            }

            if (pkt->decode()) {
                PacketBufferIterator beg(pkt);
                PacketBuffers buf(beg, bytes_recved);
                Bus::on_recv(pkt, buf);
                TunnelIArchive ar(buf);
                boost::uint16_t ver;
                boost::uint16_t tid;
                ar >> ver >> tid;
                ar.seekg(0, std::ios::beg);
                Bus::on_recv(tid, pkt, buf);
            } else {
            }

            send();

            socket_.async_receive(*pkt, 
                boost::bind(&UdpSocket::handle_recv, this, pkt, _1, _2));
        }

        void UdpSocket::send()
        {
            UdpPacket pkt;
            TunnelHeader th;
            while (true) {

            }
        }

    } // namespace client
} // namespace trip
