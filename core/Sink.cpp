// Sink.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Sink.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/PartPiece.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        Sink::Sink(
            Resource & resource)
            : resource_(&resource)
            , beg_(resource, 0)
            , pos_(beg_)
            , avl_(beg_)
            , end_(beg_)
            , off_(0)
            , size_(0)
        {
            resource_->merged.on(
                boost::bind(&Sink::on_event, this, _2));
            resource_->meta_changed.on(
                boost::bind(&Sink::on_event, this, _2));
        }

        Sink::~Sink()
        {
        }

        void Sink::seek_to(
            boost::uint64_t seg, 
            boost::uint32_t begin, 
            boost::uint32_t end)
        {
            DataId ibegin(seg, 0, begin / PIECE_SIZE);
            DataId iend(seg, 0, (end + PIECE_SIZE - 1) / PIECE_SIZE);
            if (end == 0) {
                iend = DataId((seg + 1), 0, 0);
            }
            off_ = begin % PIECE_SIZE;
            size_ = end - begin;
            if (pos_.id() != ibegin) {
                resource_->update_sink(this);
                pos_ = PieceIterator(*resource_, ibegin); 
            }
            end_ =  PieceIterator(*resource_, iend);
            resource_->seek(pos_);
        }

        Piece::pointer Sink::read()
        {
            if (pos_ == end_) {
                resource_->data_ready.un(
                    boost::bind(&Sink::on_event, this, _2));
                return NULL;
            }
            Piece::pointer p = *pos_;
            if (!p) {
                resource_->seek(pos_);
                p = *pos_;
            }
            if (p) {
                ++pos_;
                if (off_ || size_ < p->size())
                    p = PartPiece::alloc(p, off_, size_);
                off_ = 0;
                size_ -= p->size();
            } else {
                resource_->data_ready.on(
                    boost::bind(&Sink::on_event, this, _2));
            }
            return p;
        }

        bool Sink::at_end() const
        {
            return pos_ == end_;
        }

        void Sink::on_event(
            util::event::Event const & event)
        {
            if (event == resource_->data_ready) {
                bool notify = pos_ == avl_;
                avl_ = PieceIterator(*resource_, resource_->data_ready.id);
                if (notify) {
                    on_data();
                }
                if (avl_.id().segment > pos_.id().segment) {
                    resource_->data_ready.un(
                        boost::bind(&Sink::on_event, this, _2));
                }
            } else if (event == resource_->seg_meta_ready) {
                on_meta(resource_->seg_meta_ready.id.segment, *resource_->seg_meta_ready.meta);
            } else if (event == resource_->merged) {
                resource_->merged.un(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->seg_meta_ready.un(
                    boost::bind(&Sink::on_event, this, _2));
                resource_ = resource_->merged.resource;
                resource_->seg_meta_ready.on(
                    boost::bind(&Sink::on_event, this, _2));
            } else {
                on_meta(*resource_->meta_changed.meta);
                resource_->meta_changed.un(
                    boost::bind(&Sink::on_event, this, _2));
                resource_->seg_meta_ready.on(
                    boost::bind(&Sink::on_event, this, _2));
            }
        }

    } // namespace client
} // namespace trip
