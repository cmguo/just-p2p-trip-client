// UdpTunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/proto/TunnelHeader.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/net/Fifo.h"

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
            : Bus(&socket, new Fifo)
            , l_seq_(0)
            , ep_pairs_(NULL)
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
            //void * head, 
            NetBuffer & buf)
        {
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            TunnelHeader & th = pkt.th;
            TunnelOArchive ar(buf);
            th.ver = 1;
            th.tid = p_id_;
            std::streampos pos = ar.tellp();
            ar.seekp(sizeof(TunnelHeader), std::ios::cur);
            std::streampos pos2 = ar.tellp();
            /*
            while (Bus::is_signal()) {
                Bus::on_send(buf);
                std::streampos pos3 = ar.tellp();
                if (pos3 == pos2)
                    break;
                pos2 = pos3;
            }
            */
            assert(ep_pairs_);
            assert(first());
            if (ep_pairs_) {
                pkt.endpairs = ep_pairs_;
                Message * msg = NULL;
                while ((msg = (Message *)first())) {
                    // for stat, mark begin of next message
                    // ar.seekg(pos2, std::ios::beg);
                    ar << *msg;
                    if (ar) {
                        //LOG_DEBUG("[on_send] size=" << size_t(ar.tellp() - pos2) - sizeof(MessageHeader) << ", sid=" << msg->sid 
                        //    << ", type=" << msg_type_str(msg->type));
                        pop();
                        free_message(msg);
                        pos2 = ar.tellp();
                        // update send stat
                        // Bus::on_send(sid, buf);
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
            if (l_seq_ == 0) l_seq_ = 1;
            th.seq = l_seq_++;
            ar << th;
            ar.seekp(pos2, std::ios::beg);
            //LOG_DEBUG("[on_send] tid=" << l_id() << "-" << p_id() << ", seq=" << th.seq);
            Bus::on_send(/*head, */buf);
        }

        void UdpTunnel::on_recv(
            //void * head, 
            NetBuffer & buf)
        {
            Bus::on_recv(/*head, */buf);
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            TunnelHeader & th = pkt.th;
            p_seq_ = th.seq;
            size_t end = buf.pubseekoff(0, std::ios::cur, std::ios::out);
            TunnelIArchive ar(buf);
            ar >> th;
            //LOG_DEBUG("[on_recv] tid=" << l_id() << "-" << p_id() << ", seq=" << th.seq);
            size_t pos = ar.tellg();
            while (pos + 8 < end) {
                boost::uint16_t size;
                boost::uint16_t sid;
                boost::uint16_t type;
                buf.pubseekoff(pos + 8, std::ios::beg, std::ios::out);
                ar >> size >> sid >> type;
                ar.seekg(pos, std::ios::beg);
                pos += (8 + size);
                buf.pubseekoff(pos, std::ios::beg, std::ios::out); // limit in this message
                //LOG_DEBUG("[on_recv] size=" << size << ", sid=" << sid << ", type=" << msg_type_str(type));
                Bus::on_recv(sid, /*head, */buf);
                ar.seekg(pos, std::ios::beg);
            }
        }

        void UdpTunnel::on_timer(
            Time const & now)
        {
            Bus::on_timer(now);
        }

    } // namespace client
} // namespace trip
