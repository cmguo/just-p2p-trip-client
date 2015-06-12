// UdpSocket.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/Error.h"
#include "trip/client/proto/PacketBuffers.h"
#include "trip/client/proto/TunnelHeader.h"
#include "trip/client/timer/TimerManager.h"

#include <util/buffers/RefBuffers.h>
#include <util/daemon/Daemon.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpSocket", framework::logger::Debug);

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
            if (ec) {
                LOG_ERROR("[start] bind failed, ec:" << ec.message());
                return false;
            }
            for (size_t i = 0; i < parallel; ++i) {
                UdpPacket * pkt = new UdpPacket;
                socket_.async_receive_from(util::buffers::ref_buffers(*pkt), pkt->endp, 
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
            if (stopped_) {
                LOG_DEBUG("[handle_recv] stopped");
                delete pkt;
                return;
            }

            if (!ec && pkt->decode()) {
                PacketBufferIterator beg(pkt);
                PacketBuffers buf(beg, bytes_recved);
                Bus::on_recv(pkt, buf);
                TunnelIArchive ar(buf);
                boost::uint16_t ver;
                boost::uint16_t tid;
                ar >> ver >> tid;
                ar.seekg(0, std::ios::beg);
                Bus::on_recv(tid, pkt, buf);
            } else if (!ec) {
                ec = udp_error::chksum_error;
                LOG_ERROR("[handle_recv] failed, ec:" << ec.message());
            } else {
                LOG_ERROR("[handle_recv] failed, ec:" << ec.message());
            }

            send();

            socket_.async_receive_from(util::buffers::ref_buffers(*pkt), pkt->endp, 
                boost::bind(&UdpSocket::handle_recv, this, pkt, _1, _2));
        }

        void UdpSocket::send()
        {
            UdpPacket pkt;
            TunnelHeader th;
            while (true) {

            }
        }

        void UdpSocket::handle_timer(
            framework::timer::Time const & now)
        {
            on_timer(now);
        }

    } // namespace client
} // namespace trip
