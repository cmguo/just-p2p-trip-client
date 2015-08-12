// UdpTunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/proto/TunnelHeader.h"
#include "trip/client/net/Queue.h"

#define SHARER_QUEUE_SIZE 16

namespace trip
{
    namespace client
    {

        UdpTunnel::UdpTunnel(
            UdpSocket & socket)
            : Tunnel(&socket)
            , tid_(0)
            , seq_(0)
        {
        }

        UdpTunnel::~UdpTunnel()
        {
        }

        UdpSession * UdpTunnel::main_session()
        {
            return static_cast<UdpSession *>(slot_at(uint16_t(0))->cell);
        }

        void UdpTunnel::on_send(
            void * head, 
            NetBuffer & buf)
        {
            TunnelOArchive ar(buf);
            size_t pos = ar.tellp();
            TunnelHeader th;
            th.tid = tid_;
            th.seq = seq_++;
            ar.seekp(sizeof(TunnelHeader), std::ios::cur);
            size_t pos2 = ar.tellp();
            while (true) {
                boost::uint16_t id = 0;
                Tunnel::on_send(id, &th, buf);
                if (pos2 == ar.tellp())
                    break;
                pos2 = ar.tellp();
            }
            ar.seekp(pos, std::ios::beg);
            if (pos2 == pos + sizeof(TunnelHeader))
                return;
            ar << th;
            ar.seekp(pos2, std::ios::beg);
            Tunnel::on_send(head, buf);
        }

        void UdpTunnel::on_recv(
            void * head, 
            NetBuffer & buf)
        {
            Tunnel::on_recv(head, buf);
            TunnelIArchive ar(buf);
            TunnelHeader th; 
            ar >> th;
            while (buf.in_avail()) {
                size_t pos = ar.tellg();
                boost::uint16_t size;
                boost::uint16_t id;
                ar >> size >> id;
                ar.seekg(pos, std::ios::beg);
                Tunnel::on_recv(id, &th, buf);
            }
        }

        void UdpTunnel::on_timer()
        {
        }

    } // namespace client
} // namespace trip
