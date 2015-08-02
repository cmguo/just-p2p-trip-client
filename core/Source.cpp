// Source.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Scheduler.h"

namespace trip
{
    namespace client
    {

        Source::Source(
            Resource & resource, 
            Url const & url)
            : resource_(resource)
            , url_(url)
        {
        }

        Source::~Source()
        {
        }

        void Source::attach(
            Scheduler & scheduler)
        {
            scheduler_ = &scheduler;
            open(url_);
        }

        void Source::detach()
        {
            close();
        }

        Url const & Source::url() const
        {
            return url_;
        }

        bool Source::attached() const
        {
            return scheduler_ != NULL;
        }

        bool Source::request(
            std::vector<DataId> const & pieces)
        {
            Time now;
            for (size_t i = 0; i < pieces.size(); ++i) {
                requests_.push_back(Request(pieces[i], now));
                now += delta_;
            }
            return do_request(pieces);
        }

        void Source::on_ready()
        {
            std::vector<DataId> pieces;
            if (scheduler_->get_task(*this, pieces)) {
                request(pieces);
            }
        }

        void Source::on_map(
            DataId id, 
            boost::dynamic_bitset<> & map)
        {
            map_id_ = id;
            map_.swap(map);
        }

        void Source::on_data(
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
            resource_.set_piece(id, piece);
            // scheduler_->on_piece(id);
            on_ready();
        }

        void Source::on_timer(
            Time const & now)
        {
            Time time = now - rtt_;
            std::deque<Request>::iterator iter = requests_.begin();
            while (iter != requests_.end() && iter->time < time) {
                // scheduler_->on_timeout(iter->id);
                ++iter;
            }
			scheduler_->on_timeout(std::vector<DataId>(requests_.begin(), iter));
            requests_.erase(requests_.begin(), iter);
        }

    } // namespace client
} // namespace trip
