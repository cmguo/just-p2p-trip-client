// Source.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Scheduler.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Source", framework::logger::Debug);

        Source::Source(
            Resource & resource, 
            Url const & url)
            : resource_(resource)
            , url_(url)
            , ctx_(NULL)
        {
        }

        Source::~Source()
        {
        }

        void Source::attach(
            Scheduler & scheduler)
        {
            LOG_INFO("[attach]");
            scheduler_ = &scheduler;
            open(url_);
        }

        void Source::detach()
        {
            LOG_INFO("[detach]");
            close();
        }

        void Source::context(
            void * ctx)
        {
            ctx_ = ctx;
        }

        void * Source::context() const
        {
            return ctx_;
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
            LOG_TRACE("[on_ready]");
            std::vector<DataId> pieces;
            if (scheduler_->get_task(*this, pieces)) {
                request(pieces);
            }
        }

        void Source::on_segment_meta(
            DataId id, 
            SegmentMeta const & meta)
        {
            //LOG_TRACE("[on_segment_meta]");
            resource_.set_segment_meta(id, meta);
        }

        void Source::on_data(
            DataId id, 
            Piece::pointer piece)
        {
            //LOG_INFO("[on_data] id=" << id.id);
            resource_.set_piece(id, piece);
            // scheduler_->on_piece(id);
        }

        void Source::on_timeout(
            DataId id)
        {
            scheduler_->on_timeout(id);
        }

    } // namespace client
} // namespace trip
