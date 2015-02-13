// Sink.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Sink.h"
#include "trip/client/core/Resource.h"

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

        bool Sink::get_meta(
            ResourceMeta & meta,
            boost::system::error_code & ec)
        {
            meta = resource_.meta();
            return true;
        }

        bool Sink::get_segment_meta(
            SegmentMeta & meta, 
            boost::system::error_code & ec)
        {
        }

        bool Sink::get_stat(
            ResourceStat & stat, 
            boost::system::error_code & ec)
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
                request_.end = MAKE_ID(seg + 1, 0, 0);
            }
            off_ = begin % PIECE_SIZE;
            size_ = end - begin;
        }

        Piece::pointer Sink::read()
        {
            if (iter_ == end_)
                return NULL;
            Piece::pointer p = *iter;
            if (!p) {
                iter = resource_.iterator_at(request_.begin);
                p = *iter;
            }
            ++iter;
            if (off_ || size_ < piece.size())
                p = PartPiece::alloc(p, off_, size_);
            off_ = 0;
            size_ -= p->size();
            return p;
        }

        bool Sink::bump();

    } // namespace client
} // namespace trip
