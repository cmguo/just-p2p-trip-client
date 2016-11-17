// Sink.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Sink.h"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/PartPiece.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Sink", framework::logger::Debug);

        Sink::Sink(
            Resource & resource, 
            Url const & url)
            : resource_(&resource)
            , url_(url)
            , scheduler_(NULL)
            , off_(0)
            , off2_(0)
            , lock_(NULL)
        {
        }

        Sink::~Sink()
        {
        }

        Resource const & Sink::resource() const
        {
            return *resource_;
        }

        Url const & Sink::url() const
        {
            return url_;
        }

        void Sink::attach(
            Scheduler & scheduler)
        {
            LOG_TRACE("[attach]");
            scheduler_ = &scheduler;
            resource_ = &scheduler_->resource();
            resource_->seg_meta_ready.on(
                boost::bind(&Sink::on_event, this, _2));
            lock_ = resource_->alloc_lock(beg_.id(), end_.id());
            on_attach();
        }

        void Sink::detach()
        {
            LOG_TRACE("[detach]");
            on_detach();
            resource_->release_lock(lock_);
            scheduler_ = NULL;
        }

        void Sink::set_error(
            boost::system::error_code const & ec)
        {
            error_ = ec;
            on_error(ec);
        }

        bool Sink::attached() const
        {
            return scheduler_ != NULL;
        }

        void Sink::close()
        {
            if (scheduler_)
                scheduler_->del_sink(this);
        }

        void Sink::seek_to(
            boost::uint64_t seg, 
            boost::uint32_t begin, 
            boost::uint32_t end)
        {
            LOG_TRACE("[seek_to] seg=" << seg);
            DataId ibegin(seg, 0, begin / PIECE_SIZE);
            DataId iend(seg, 0, (end + PIECE_SIZE - 1) / PIECE_SIZE);
            if (end == 0) {
                iend.inc_segment();
            }
            DataId pos = pos_.id();
            beg_ = resource_->iterator_at(ibegin);
            end_ = resource_->iterator_at(iend, false);
            pos_ = beg_; 
            off_ = begin % PIECE_SIZE;
            off2_ = end % PIECE_SIZE;
            if (pos != ibegin) {
                scheduler_->update_sink(this);
            }
            resource_->modify_lock(lock_, ibegin, iend);
        }

        void Sink::seek_end(
            boost::uint64_t seg)
        {
            LOG_TRACE("[seek_end] seg=" << seg);
            resource_->data_ready.un(
                boost::bind(&Sink::on_event, this, _2));
        }

        Piece::pointer Sink::read()
        {
            assert(!at_end());
            Piece::pointer p = *pos_;
            if (!p) {
                resource_->update(pos_);
                p = *pos_;
            }
            if (p) {
                //LOG_DEBUG("[read] pos=" << pos_.id().block_piece);
                ++pos_;
                if (off_) {
                    p = PartPiece::alloc(p, off_, p->size() - off_);
                    off_ = 0;
                }
                if (pos_ == end_ && off2_)
                    p = PartPiece::alloc(p, 0, off2_);
            } else {
                //LOG_INFO("[read] would block");
                resource_->data_ready.on(
                    boost::bind(&Sink::on_event, this, _2));
            }
            return p;
        }

        ResourceMeta const * Sink::meta()
        {
            return resource_ ? resource_->meta() : NULL;
        }

        boost::system::error_code Sink::error()
        {
            return error_;
        }

        SegmentMeta const * Sink::segment_meta(
            boost::uint64_t segm)
        {
            return resource_ ? resource_->get_segment_meta(DataId(segm, 0, 0)) : NULL;
        }

        bool Sink::at_end() const
        {
            return pos_.id() >= end_.id();
        }

        DataId const & Sink::position() const
        {
            return pos_.id();
        }

        void Sink::on_event(
            util::event::Event const & event)
        {
            if (event == resource_->data_ready) {
                //LOG_INFO("[on_event] data_ready, id=" << resource_->data_ready.id);
                resource_->data_ready.un(
                    boost::bind(&Sink::on_event, this, _2));
                on_data();
            } else if (event == resource_->seg_meta_ready) {
                if (pos_.id().segment == resource_->seg_meta_ready.id.segment) {
                    LOG_TRACE("[on_event] seg_meta_ready, segment=" << resource_->seg_meta_ready.id.segment);
                    on_segment_meta(resource_->seg_meta_ready.id.segment, *resource_->seg_meta_ready.meta);
                }
            }
        }

    } // namespace client
} // namespace trip
