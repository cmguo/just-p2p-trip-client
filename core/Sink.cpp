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
            : resource_(resource)
            , iter_(resource, 0)
            , end_(iter_)
            , off_(0)
            , size_(0)
        {
        }

        Sink::~Sink()
        {
        }

        void Sink::seek_to(
            boost::uint64_t seg, 
            boost::uint32_t begin, 
            boost::uint32_t end)
        {
            request_.type = 0;
            request_.begin = MAKE_ID(seg, 0, begin / PIECE_SIZE);
            if (end) {
                request_.end = MAKE_ID(seg, 0, (end + PIECE_SIZE - 1) / PIECE_SIZE);
            } else {
                request_.end = MAKE_ID((seg + 1), 0, 0);
            }
            off_ = begin % PIECE_SIZE;
            size_ = end - begin;
            if (iter_.id() != request_.begin) {
                resource_.update_sink(this);
            }
            iter_ = resource_.iterator_at(request_.begin);
        }

        Piece::pointer Sink::read()
        {
            if (iter_ == end_) {
                resource_.data_ready.un(
                    boost::bind(&Sink::on_event, this, _2));
                return NULL;
            }
            Piece::pointer p = *iter_;
            if (!p) {
                iter_ = resource_.iterator_at(request_.begin);
                p = *iter_;
            }
            ++iter_;
            if (off_ || size_ < p->size())
                p = PartPiece::alloc(p, off_, size_);
            off_ = 0;
            size_ -= p->size();
            return p;
        }

        bool Sink::at_end() const
        {
            return iter_.id() == request_.end;
        }

        void Sink::on_event(
            util::event::Event const & event)
        {
            if (event == resource_.data_ready) {
                bool notify = iter_ == end_;
                end_ = PieceIterator(resource_, resource_.data_ready.id);
                if (notify) {
                    on_data();
                }
                if (end_.id() - iter_.id() > PIECE_PER_BLOCK) {
                    resource_.data_ready.un(
                        boost::bind(&Sink::on_event, this, _2));
                }
            }
        }

    } // namespace client
} // namespace trip
