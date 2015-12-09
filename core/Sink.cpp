// Sink.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Sink.h"
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
            Resource & resource)
            : resource_(&resource)
            , off_(0)
            , off2_(0)
            , lock_(NULL)
        {
            resource_->merged.on(
                boost::bind(&Sink::on_event, this, _2));
            resource_->meta_changed.on(
                boost::bind(&Sink::on_event, this, _2));
            resource_->data_seek.on(
                boost::bind(&Sink::on_event, this, _2));
            lock_ = resource_->alloc_lock(beg_.id(), end_.id());
        }

        Sink::~Sink()
        {
            resource_->del_sink(this);
            resource_->release_lock(lock_);
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
                resource_->update_sink(this);
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

        bool Sink::at_end() const
        {
            return pos_.id() >= end_.id();
        }

        DataId Sink::position() const
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
            } else if (event == resource_->merged) {
                LOG_TRACE("[on_event] merged, id=" << resource_->merged.resource->id());
                resource_->del_sink(this);
                resource_->merged.un(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->seg_meta_ready.un(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->release_lock(lock_);
                resource_ = resource_->merged.resource;
                DataId id;
                lock_ = resource_->alloc_lock(id, id);
                resource_->seg_meta_ready.on(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->add_sink(this);
            } else if (event == resource_->data_seek) {
                LOG_TRACE("[on_event] data_seek, id=" << resource_->data_seek.id);
                //if (!at_end() && resource_->data_seek.id == beg_.id()) {
                //    on_data();
                //}
            } else if (event == resource_->meta_changed) {
                LOG_TRACE("[on_event] meta_changed");
                on_meta(*resource_->meta_changed.meta);
                resource_->meta_changed.un(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->seg_meta_ready.on(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->add_sink(this);
            }
        }

    } // namespace client
} // namespace trip
