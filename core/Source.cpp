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
            , scheduler_(NULL)
            , ctx_(NULL)
        {
        }

        Source::~Source()
        {
            if (scheduler_)
                scheduler_->on_source_lost(*this);
            scheduler_ = NULL;
        }

        void Source::attach(
            Scheduler & scheduler)
        {
            LOG_TRACE("[attach]");
            scheduler_ = &scheduler;
            open(url_);
        }

        void Source::detach()
        {
            LOG_TRACE("[detach]");
            close();
            scheduler_ = NULL;
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

        Resource & Source::resource() const
        {
            return resource_;
        }

        Url const & Source::url() const
        {
            return url_;
        }

        bool Source::attached() const
        {
            return scheduler_ != NULL;
        }

        void Source::seek(
            DataId id)
        {
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
            //LOG_INFO("[on_data] id=" << id);
            if (!resource_.set_piece(id, piece)) {
                LOG_WARN("[on_data] set piece failed id=" << id << " piece_size=" << piece->size());
            }
            // scheduler_->on_piece(id);
        }

        void Source::on_timeout(
            DataId id)
        {
            scheduler_->on_timeout(id);
        }

    } // namespace client
} // namespace trip
