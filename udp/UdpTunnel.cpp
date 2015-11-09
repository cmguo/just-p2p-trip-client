// UdpTunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/proto/TunnelHeader.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/net/Queue.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#define SHARER_QUEUE_SIZE 16

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpTunnel", framework::logger::Debug);

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

        void UdpTunnel::set_endpoint(
            framework::network::Endpoint const & ep)
        {
            endpoint_ = ep;
        }

        void UdpTunnel::on_send(
            void * head, 
            NetBuffer & buf)
        {
            UdpPacket * pkt = (UdpPacket *)head;
            TunnelHeader & th = pkt->th;
            TunnelOArchive ar(buf);
            th.ver = 1;
            th.tid = tid_;
            std::streampos pos = ar.tellp();
            ar.seekp(sizeof(TunnelHeader), std::ios::cur);
            std::streampos pos2 = ar.tellp();
            while (Bus::is_signal()) {
                Bus::on_send(head, buf);
                std::streampos pos3 = ar.tellp();
                if (pos3 == pos2)
                    break;
                pos2 = pos3;
            }
            if (endpoint_.port()) {
                pkt->endp = endpoint_;
                Message * msg = NULL;
                while ((msg = (Message *)first())) {
                    ar << *msg;
                    if (ar) {
                        //LOG_DEBUG("[on_send] size=" << size_t(ar.tellp() - pos2) - sizeof(MessageHeader) << ", sid=" << msg->sid);
                        pop();
                        free_message(msg);
                        pos2 = ar.tellp();
                    } else {
                        ar.clear();
                        break;
                    }
                }
            }
            ar.seekp(pos, std::ios::beg);
            if (pos2 == pos + (std::streamoff)sizeof(TunnelHeader)) {
                return;
            }
            if (seq_ == 0) seq_ = 1;
            th.seq = seq_++;
            ar << th;
            ar.seekp(pos2, std::ios::beg);
            //LOG_DEBUG("[on_send] tid=" << th.tid << ", seq=" << th.seq);
            Tunnel::on_send(head, buf);
        }

        void UdpTunnel::on_recv(
            void * head, 
            NetBuffer & buf)
        {
            Tunnel::on_recv(head, buf);
            UdpPacket * pkt = (UdpPacket *)head;
            TunnelHeader & th = pkt->th;
            size_t end = buf.pubseekoff(0, std::ios::cur, std::ios::out);
            TunnelIArchive ar(buf);
            ar >> th;
            //LOG_DEBUG("[on_recv] tid=" << th.tid << ", seq=" << th.seq);
            size_t pos = ar.tellg();
            while (pos + 8 < end) {
                boost::uint16_t size;
                boost::uint16_t sid;
                buf.pubseekoff(pos + 8, std::ios::beg, std::ios::out);
                ar >> size >> sid;
                ar.seekg(pos, std::ios::beg);
                pos += (8 + size);
                buf.pubseekoff(pos, std::ios::beg, std::ios::out); // limit in this message
                //LOG_DEBUG("[on_recv] size=" << size << ", sid=" << sid);
                Tunnel::on_recv(sid, head, buf);
                ar.seekg(pos, std::ios::beg);
            }
        }

        void UdpTunnel::on_timer()
        {
        }

    } // namespace client
} // namespace trip
