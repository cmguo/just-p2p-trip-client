// UdpTunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpSessionListener.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/proto/TunnelHeader.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/net/Fifo.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#define SHARER_QUEUE_SIZE 16

#if 1
#  undef LOG_DEBUG
#  define LOG_DEBUG(xxx)
#endif

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

        bool UdpTunnel::is_open() const
        {
            return p_id() != 0;
        }

        Uuid const & UdpTunnel::pid() const
        {
            return static_cast<UdpSessionListener *>(slot_at(uint16_t(0))->cell)->endpoint().id;
        }

        void UdpTunnel::on_send(
            NetBuffer & buf)
        {
            assert(ep_pairs_);
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            TunnelHeader & th = pkt.th;
            TunnelOArchive ar(buf);
            th.ver = 1;
            th.tid = p_id_;
            pkt.endpairs = ep_pairs_;
            std::streampos pos = ar.tellp();
            ar.seekp(sizeof(TunnelHeader), std::ios::cur);
            std::streampos pos2 = ar.tellp();
            while (Bus::is_signal()) {
                Bus::send_signal(buf);
                pos2 = ar.tellp();
            }
            Cell * c = NULL;
            Message * msg = NULL;
            while ((msg = (Message *)first(c))) {
                // for stat, mark begin of next message
                buf.pubseekpos(pos2, std::ios::in);
                ar << *msg;
                if (ar) {
                    LOG_DEBUG("[on_send] size=" << size_t(ar.tellp() - pos2) - sizeof(MessageHeader) << ", sid=" << msg->sid 
                        << ", type=" << msg_type_str(msg->type));
                    pop();
                    free_message(msg);
                    pos2 = ar.tellp();
                    // update send stat
                    Bus::on_send(c, buf);
                } else {
                    ar.clear();
                    break;
                }
            }
            // restore read pos
            buf.pubseekpos(pos, std::ios::in);
            ar.seekp(pos, std::ios::beg);
            if (pos2 == pos + (std::streamoff)sizeof(TunnelHeader)) {
                assert(false);
                return;
            }
            if (l_seq_ == 0) l_seq_ = 1;
            th.seq = l_seq_++;
            ar << th;
            ar.seekp(pos2, std::ios::beg);
            LOG_DEBUG("[on_send] tid=" << l_id() << " -> " << p_id() << ", seq=" << th.seq);
            Bus::on_send(buf);
        }

        void UdpTunnel::on_recv(
            NetBuffer & buf)
        {
            Bus::on_recv(buf);
            UdpPacket & pkt = static_cast<UdpPacket &>(buf);
            TunnelHeader & th = pkt.th;
            p_seq_ = th.seq;
            size_t end = buf.pubseekoff(0, std::ios::cur, std::ios::out);
            TunnelIArchive ar(buf);
            ar >> th;
            LOG_DEBUG("[on_recv] tid=" << l_id() << " <- " << p_id() << ", seq=" << th.seq);
            size_t pos = ar.tellg();
            while (pos + 8 < end) {
                boost::uint16_t size;
                boost::uint16_t sid;
                buf.pubseekoff(pos + 8, std::ios::beg, std::ios::out);
                ar >> size >> sid;
                ar.seekg(pos, std::ios::beg);
                pos += (8 + size);
                buf.pubseekoff(pos, std::ios::beg, std::ios::out); // limit in this message
                Bus::on_recv(sid, buf);
                ar.seekg(pos, std::ios::beg);
            }
        }

        void UdpTunnel::on_connecting()
        {
            array_t::value_const_iterator iter = array_.value_cbegin();
            for (; iter != array_.value_cend(); ++iter) {
                if (iter->flags & Slot::sfActive)
                    static_cast<UdpSession *>(iter->cell)->on_tunnel_connecting();
            }
        }

        void UdpTunnel::on_disconnect()
        {
            Bus::p_id(0);
            array_t::value_const_iterator iter = array_.value_cbegin();
            for (; iter != array_.value_cend(); ++iter) {
                if (iter->flags & Slot::sfActive)
                    static_cast<UdpSession *>(iter->cell)->on_tunnel_disconnect();
            }
        }

        void UdpTunnel::on_timer(
            Time const & now)
        {
            Bus::on_timer(now);
        }

    } // namespace client
} // namespace trip
