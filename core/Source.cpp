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
            open();
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

        void Source::on_ready()
        {
            std::vector<DataId> pieces;
            if (scheduler_->get_task(*this, pieces))
                request(pieces);
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
            resource_.set_piece(id, piece);
            on_ready();
        }

    } // namespace client
} // namespace trip
