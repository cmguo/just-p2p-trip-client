// P2pSource.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pSource::P2pSource(
            Resource & resource,
            UdpTunnel & tunnel, 
            Url const & url)
            : Source(resource, url)
            , UdpSession(tunnel)
        {
            MessageRequestBind req;
            req.rid = resource.id();
            req.sid = id();
            send_msg(new Message(req));
        }

        P2pSource::~P2pSource()
        {
        }

        bool P2pSource::open(
            Url const & url)
        {
            return false;
        }

        bool P2pSource::close()
        {
            return false;
        }

        bool P2pSource::request(
            std::vector<DataId> const & pieces)
        {
            Time now;
            for (size_t i = 0; i < pieces.size(); ++i) {
                requests_.push_back(Request(pieces[i], now));
                now += delta_;
            }
            return true;
        }

        bool P2pSource::has_segment(
            DataId sid) const
        {
            if (sid < map_id_)
                return false;
            boost::uint64_t index = sid.segment - map_id_.segment;
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
            return window_size() - requests_.size();
        }

        void P2pSource::on_data(
            DataId id, 
            Piece::pointer piece)
        {
            Time now;
            std::deque<Request>::iterator iter = 
                std::find(requests_.begin(), requests_.end(), Request(id, now));
            if (iter == requests_.end())
                return;
            if (iter == requests_.begin()) {
                ++iter;
                while (iter != requests_.end() && iter->id == DataId(MAX_SEGMENT, 0, 0)) 
                    ++iter;
                requests_.erase(requests_.begin(), iter);
            } else {
                iter->id = DataId(MAX_SEGMENT, 0, 0);
            }
            Source::on_data(id, piece);
            Source::on_ready();
        }

        void P2pSource::on_map(
            DataId id, 
            boost::dynamic_bitset<> & map)
        {
            map_id_ = id;
            map_.swap(map);
        }

        void P2pSource::on_timer(
            Time const & now)
        {
            Time time = now - rtt_;
            std::deque<Request>::iterator iter = requests_.begin();
            while (iter != requests_.end() && iter->time < time) {
                if (iter->id.top == 0)
                    on_timeout(iter->id);
                ++iter;
            }
            requests_.erase(requests_.begin(), iter);
        }

        void P2pSource::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case RSP_Data:
                break;
            case RSP_Map:
                break;
            case RSP_Meta:
                break;
            case RSP_Bind:
                break;
            case RSP_Unbind:
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
