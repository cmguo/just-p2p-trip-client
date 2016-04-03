// P2pSource.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#define PREPARE_MAP_RANGE 32

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.P2pSource", framework::logger::Debug);

        P2pSource::P2pSource(
            P2pManager & manager, 
            Resource & resource,
            UdpTunnel & tunnel, 
            Url const & url)
            : Source(resource, url)
            , UdpSession(tunnel)
            , manager_(manager)
            , map_id_(0)
            , delta_(Duration::milliseconds(50))
            , rtt_(Duration::milliseconds(500))
            , map_req_(0, Time())
            , req_count_(0)
        {
        }

        P2pSource::~P2pSource()
        {
            manager_.del_source(this);
        }

        bool P2pSource::open(
            Url const & url)
        {
            if (tunnel().is_open())
                return open();
            return false;
        }

        bool P2pSource::open()
        {
            Message * msg = alloc_message();
            MessageRequestBind & req = 
                msg->get<MessageRequestBind>();
            req.rid = resource_.id();
            req.sid = l_id();
            map_req_.time = Time() + Duration::seconds(1);
            return send_msg(msg);
        }

        bool P2pSource::close()
        {
            std::deque<Request>::iterator iter = requests_.begin();
            while (iter != requests_.end()) {
                if (iter->id.top == 0) {
                    //LOG_INFO("[on_timer] timeout piece, id=" << iter->id);
                    --req_count_;
                    on_timeout(iter->id);
                }
                ++iter;
            }
            requests_.clear();

            if (attached())
                return true;
            Message * msg = alloc_message();
            MessageRequestUnbind & req = 
                msg->get<MessageRequestUnbind>();
            (void)req;
            return send_msg(msg);
        }

        bool P2pSource::request(
            std::vector<DataId> const & pieces)
        {
            assert(!pieces.empty());
            Time now;
            Message * msg = alloc_message();
            MessageRequestData & req = 
                msg->get<MessageRequestData>();
            //LOG_DEBUG("[request] id=" << pieces[0]);
            requests_.push_back(Request(pieces[0], now));
            boost::uint64_t index = pieces[0].id;
            req.index = index;
            for (size_t i = 1; i < pieces.size(); ++i) {
                //LOG_DEBUG("[request] id=" << pieces[i]);
                requests_.push_back(Request(pieces[i], now));
                now += delta_;
                if (i > 0) {
                    req.offsets.push_back(pieces[i].id - index);
                }
                index = pieces[i].id;
            }
            req_count_ += (boost::uint32_t)pieces.size();
            return send_msg(msg);
        }

        void P2pSource::cancel()
        {
            requests_.clear();
            req_count_ = 0;
        }

        void P2pSource::seek(
            DataId id)
        {
            if (p_id() == 0)
                return;
            req_map(id);
        }

        bool P2pSource::has_segment(
            DataId id) const
        {
            if (id < map_id_)
                return false;
            boost::uint64_t index = id.segment - map_id_.segment;
            return index < map_.size() && map_[(size_t)index];
        }

        bool P2pSource::has_block(
            DataId bid) const
        {
            return false;
        }

        boost::uint32_t P2pSource::window_size() const
        {
            return rtt_.total_milliseconds() / delta_.total_milliseconds();
        }

        boost::uint32_t P2pSource::window_left() const
        {
            return window_size() - req_count_;
        }


        void P2pSource::req_map(
            DataId id)
        {
            if (id.top)
                id = resource_.data_ready.id;
            id.inc_segment(0);
            if (map_.empty() || id < map_id_ 
                || id > map_id_ + DataId::segments(PREPARE_MAP_RANGE / 2)) {
                // retry if no response during one second
                LOG_DEBUG("[req_map] update map, from segno=" << id);
                map_req_.id = id;
                map_req_.time = Time() + Duration::seconds(1);
                Message * msg = alloc_message();
                MessageRequestMap & req = 
                    msg->get<MessageRequestMap>();
                req.start = id.id;
                req.count = PREPARE_MAP_RANGE;
                send_msg(msg);
            } else {
                // we still have more than half valid map range
                // start next check after 10 seconds with "resource_.data_ready.id"
                map_req_.id.top = 1;
                map_req_.time = Time() + Duration::seconds(10);
            }
        }

        void P2pSource::on_data(
            DataId id, 
            Piece::pointer piece)
        {
            Time now;
            //LOG_DEBUG("[on_data] id=" << id);
            std::deque<Request>::iterator iter = 
                std::find(requests_.begin(), requests_.end(), Request(id, now));
            if (iter == requests_.end()) {
                LOG_INFO("[on_data] unexpet piece, id=" << id);
                return;
            }
            if (iter->id.top) {
                LOG_INFO("[on_data] repeat piece, id=" << id);
                return;
            }
            if (iter == requests_.begin()) {
                ++iter;
                while (iter != requests_.end() && iter->id == DataId(MAX_SEGMENT, 0, 0)) 
                    ++iter;
                requests_.erase(requests_.begin(), iter);
            } else {
                iter->id = DataId(MAX_SEGMENT, 0, 0);
            }
            --req_count_;
            Source::on_data(id, piece);
            Source::on_ready();
        }

        void P2pSource::on_map(
            DataId id, 
            boost::dynamic_bitset<boost::uint32_t> & map)
        {
            if (id == map_req_.id || map_.empty()) {
                map_id_ = id;
                map_.swap(map);
                if (requests_.empty())
                    Source::on_ready();
            }
            // start next check after 10 seconds with "resource_.data_ready.id"
            map_req_.id.top = 1;
            map_req_.time = Time() + Duration::seconds(10);
        }

        void P2pSource::on_timer(
            Time const & now)
        {
            Cell::on_timer(now);

            Time time = now - rtt_;
            
            if (time > map_req_.time) {
                if (!is_open()) {
                    if (tunnel().is_open()) {
                        LOG_DEBUG("[on_timer] retry bind");
                        open(Url());
                    }
                    return;
                }
                req_map(map_req_.id);
            }

            std::deque<Request>::iterator iter = requests_.begin();
            while (iter != requests_.end() && iter->time < time) {
                if (iter->id.top == 0) {
                    //LOG_INFO("[on_timer] timeout piece, id=" << iter->id);
                    --req_count_;
                    on_timeout(iter->id);
                }
                ++iter;
            }
            if (iter != requests_.begin()) {
                requests_.erase(requests_.begin(), iter);
                Source::on_ready();
            }
        }

        void P2pSource::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case RSP_Data:
                {
                    MessageResponseData const & rsp =
                        msg->as<MessageResponseData>();
                    on_data(rsp.index, rsp.piece);
                }
                break;
            case RSP_Map:
                {
                    MessageResponseMap /*const*/ & rsp =
                        msg->as<MessageResponseMap>();
                    on_map(rsp.start, rsp.map);
                }
                break;
            case RSP_Meta:
                {
                }
                break;
            case RSP_Bind:
                {
                    MessageResponseBind const & rsp =
                        msg->as<MessageResponseBind>();
                    if (p_id() == 0)
                        LOG_DEBUG("[on_msg] bind succeed");
                    UdpSession::p_id(rsp.sid);
                    req_map(resource_.data_ready.id);
                }
                break;
            case RSP_Unbind:
                {
                }
                break;
            default:
                assert(false);
                break;
            }
            free_message(msg);
        }

        void P2pSource::on_tunnel_connecting()
        {
            UdpSession::on_tunnel_connecting();
            if (attached())
                open();
        }

        void P2pSource::on_tunnel_disconnect()
        {
            UdpSession::on_tunnel_disconnect();
            close();
        }

    } // namespace client
} // namespace trip
