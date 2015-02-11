// ResourceData.cpp

#include "trip/client/Common.h"
#include "trip/client/core/ResourceData.h"

namespace trip
{
    namespace client
    {

        ResourceData::ResourceData()
            : next_(0)
            , end_(0)
        {
        }

        ResourceData::~ResourceData()
        {
        }

        boost::uint16_t ResourceData::piece_size(
            boost::uint64_t id) const
        {
            return get_segment(id).piece_size(id);
        }

        Piece::pointer ResourceData::get_piece(
            boost::uint64_t id)
        {
            Piece::pointer p = get_segment(id).get_piece(id);
            if (p == NULL) {
                data_miss.id = id;
                raise(data_miss);
                p = get_segment(id).get_piece(id);
            }
            return p;
        }

        PieceIterator ResourceData::iterator_at(
            boost::uint64_t id)
        {
            PieceIterator iterator(*this, id);
            iterator.segment_ = &get_segment(id);
            iterator.block_ = &iterator.segment_->get_block(id);
            iterator.piece_ = iterator.block_->get_piece(id);
            return iterator;
        }

        PieceIterator ResourceData::end()
        {
            PieceIterator iterator(*this, MAKE_ID(MAX_SEGMENT, MAX_BLOCK, MAX_PIECE));
            return iterator;
        }

        void ResourceData::increment(
            PieceIterator & iterator)
        {
            ++iterator.id_;
            boost::uint16_t pid = PIECE(iterator.id_);
            if (pid >= iterator.block_->pieces().size()) {
                pid = 0;
                boost::uint16_t bid = BLOCK(iterator.id_) + 1;
                if (bid >= iterator.segment_->blocks().size()) {
                    bid = 0;
                    boost::uint64_t sid = SEGMENT(iterator.id_) + 1;
                    if (sid >= end_) {
                        sid = MAX_SEGMENT;
                    }
                    iterator.id_ = MAKE_ID(SEGMENT(sid), bid, 0);
                    iterator.segment_ = &get_segment(iterator.id_);
                }
                iterator.block_ = iterator.segment_->blocks()[bid];
                iterator.id_ = MAKE_ID(SEGMENT(iterator.id_), bid, 0);
            }
            iterator.piece_ = iterator.block_->pieces()[pid];
        }

        ResourceData::lock_t ResourceData::alloc_lock(
            boost::uint64_t from, 
            boost::uint64_t to)
        {
            Lock * l = new Lock;
            l->from = from;
            l->to = to;
            locks_.insert(l);
            return l;
        }

        void ResourceData::modify_lock(
            lock_t lock, 
            boost::uint64_t from, 
            boost::uint64_t to)
        {
            Lock * l = (Lock * )lock;
            locks_.erase(l);
            Lock o;
            o.from = l->from;
            o.to = l->to;
            l->from = from;
            l->to = to;
            locks_.insert(l);
            release_lock(&o);
        }

        void ResourceData::release_lock(
            lock_t lock)
        {
            Lock * l = (Lock * )lock;
            locks_.erase(l);
            release_lock(l);
            delete l;
        }

        void ResourceData::release_lock(
            Lock * l)
        {
            for (Lock * p = locks_.first(); p; p = locks_.next(p)) {
                if (p->from <= l->from) {
                    if (p->to <= l->from)
                        continue;
                    else if (p->to < l->to)
                        l->from = p->to;
                    else
                        return;
                } else if (p->from < l->to) {
                    release(l->from, p->from);
                    l->from = p->from;
                    if (p->to < l->to)
                        l->from = p->to;
                    else
                        return;
                } else {
                    release(l->from, l->to);
                    return;
                }
            }
        }

        bool ResourceData::save_segment(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta == NULL || segment.seg == NULL) {
                return false;
            }
            if (segment.seg->cache_md5sum() != segment.meta->md5sum)
                return false;
            if (segment.seg->save(path))
                return false;
            segment.saved = true;
            return true;
        }

        bool ResourceData::load_segment(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta == NULL) {
                return false;
            }
            if (Segment::file_md5sum(path) != segment.meta->md5sum)
                return false;
            segment.saved = true;
            return true;
        }

        void ResourceData::set_segment_meta(
            boost::uint64_t id, 
            SegmentMeta const & meta)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta == NULL) {
                segment.meta = new SegmentMeta(meta);
                if (segment.seg)
                    segment.seg->set_size(meta.bytesize);
                meta_ready.id = id;
                meta_ready.meta = segment.meta;
                raise(meta_ready);
            }
        }

        bool ResourceData::set_piece(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            boost::uint64_t nextid = modify_segment(id).set_piece(id, piece);
            if (nextid == MAKE_ID(0, MAX_BLOCK, 0)) {
                boost::uint64_t index = SEGMENT(id);
                if (next_ == index) {
                    while (true) {
                        ++next_;
                        if (next_ == end_) {
                            next_ = MAX_SEGMENT;
                            nextid = MAKE_ID(next_, 0, 0);
                            break;
                        }
                        Segment & segment(modify_segment(MAKE_ID(next_, 0, 0)));
                        if (!segment.finished()) {
                            nextid = MAKE_ID(next_, 0, segment.next());
                            break;
                        }
                    }
                }
            } else {
                Segment & segment(modify_segment(MAKE_ID(next_, 0, 0)));
                nextid = MAKE_ID(next_, 0, segment.next());
            }
            if (id <= nextid) {
                data_ready.id = nextid;
                raise(data_ready);
            }
            return !piece;
        }

        Block const * ResourceData::map_block(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            return modify_segment(id).map_block(id, path);
        }

        ResourceData::Segment2 const & ResourceData::get_segment2(
            boost::uint64_t id) const
        {
            boost::uint64_t index = SEGMENT(id);
            std::map<boost::uint64_t, Segment2>::const_iterator iter = 
                segments_.find(index);
            if (iter == segments_.end()) {
                static Segment2 empty_segment;
                return empty_segment;
            }
            return iter->second;
        }

        ResourceData::Segment2 & ResourceData::modify_segment2(
            boost::uint64_t id)
        {
            boost::uint64_t index = SEGMENT(id);
            std::map<boost::uint64_t, Segment2>::iterator iter = 
                segments_.find(index);
            if (iter == segments_.end()) {
                iter = segments_.insert(std::make_pair(index, Segment2())).first;
            }
            return iter->second;
        }

        Segment const & ResourceData::get_segment(
            boost::uint64_t id) const
        {
            Segment2 const & segment(get_segment2(id));
            if (segment.seg == NULL) {
                static Segment empty_segment;
                return empty_segment;
            }
            return *segment.seg;
        }

        Segment & ResourceData::modify_segment(
            boost::uint64_t id)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.seg == NULL) {
                segment.seg = new Segment(segment.meta ? segment.meta->bytesize : 0);
            }
            return *segment.seg;
        }

        void ResourceData::release(
            boost::uint64_t from,
            boost::uint64_t to)
        {
            boost::uint64_t segf = SEGMENT(from);
            boost::uint64_t segt = SEGMENT(to);
            while (segf <= segt) {
                std::map<boost::uint64_t, Segment2>::iterator iter = segments_.find(segf);
                boost::uint64_t f = from;
                ++segf;
                from = MAKE_ID(segf, 0, 0);
                if (iter == segments_.end() || iter->second.seg == NULL)
                    continue;
                if (segf == segt) {
                    iter->second.seg->release(f, to);
                } else if (BLOCK_PIECE(from) != 0) {
                    iter->second.seg->release(f, from);
                } else {
                    delete iter->second.seg;
                    iter->second.seg = NULL;
                }
            } 
        }

    } // namespace client
} // namespace trip
