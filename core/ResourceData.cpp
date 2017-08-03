// ResourceData.cpp

#include "trip/client/Common.h"
#include "trip/client/core/ResourceData.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/ResourceEvent.hpp"

#include <util/event/EventEx.hpp>

#include <framework/string/Format.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.ResourceData", framework::logger::Debug);

        ResourceData::ResourceData()
            : util::event::Observable("Resource" + framework::string::format((void const *)this)) 
            , seg_meta_ready("seg_meta_ready") 
            , data_ready("data_ready")
            , data_miss("data_miss")
            //, data_seek("data_seek")
            , segment_full("segment_full")
            , segment_error("segment_error")
            , end_(0)
            , meta_dirty_(false)
            , meta_ready_(0)
        {
            register_event(seg_meta_ready);
            register_event(data_ready);
            register_event(data_miss);
            //register_event(data_seek);
            register_event(segment_full);
        }

        ResourceData::~ResourceData()
        {
        }

        void ResourceData::get_segments(
            std::vector<SegmentMeta> & metas) const
        {
            metas.resize(end_);
            DataId id;
            for (boost::uint64_t i = 0; i < end_; ++i) {
                SegmentMeta const * meta = get_segment_meta(id);
                if (meta) metas[i] = *meta;
                id.inc_segment();
            }
        }

        Piece::pointer ResourceData::get_piece(
            DataId id, 
            size_t degree)
        {
            Segment2 const * segment2 = get_segment2(id);
            if (segment2 == NULL || segment2->meta == NULL)
                return NULL;
            Piece::pointer p;
            if (segment2->seg) {
                p = segment2->seg->get_piece(id);
            }
            if (p == NULL && segment2->saved) {
                data_miss.id = id;
                data_miss.degree = degree;
                raise(data_miss);
                if (segment2->seg) {
                    p = segment2->seg->get_piece(id);
                }
            }
            return p;
        }

        void ResourceData::get_segment_map(
            DataId from, 
            boost::uint16_t count, 
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
            boost::uint64_t start = from.top_segment;
            if (start >= end_)
                return;
            boost::uint64_t end = start + count;
            if (end > end_) end = end_;
            map.resize(end - start, false);
            segment_map_t::const_iterator iter = 
                segments_.lower_bound(start);
            while (iter != segments_.end() && iter->id < end) {
                map[iter->id - start] = iter->saved;
                ++iter;
            }
        }

        void ResourceData::get_block_map(
            DataId id, 
            boost::uint16_t count, 
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
        }

        bool ResourceData::meta_dirty() const
        {
            bool tmp = meta_dirty_;
            meta_dirty_ = false;
            return tmp;
        }

        PieceIterator ResourceData::iterator_at(
            DataId id, 
            bool update)
        {
            PieceIterator iterator(*this, id);
            if (update) {
                iterator.segment2_ = &modify_segment2(id);
                iterator.segment_ = iterator.segment2_->seg;
                if (iterator.segment_ == NULL)
                    iterator.segment_ = &modify_segment(id);
                iterator.block_ = &iterator.segment_->modify_block(id);
                if (iterator.segment2_->saved) {
                    data_miss.id = id;
                    data_miss.degree = 0;
                    raise(data_miss);
                }
                iterator.piece_ = iterator.block_->get_piece(iterator.id_);
            }
            return iterator;
        }

        void ResourceData::update(
            PieceIterator & iterator)
        {
            iterator.piece_ = iterator.block_->get_piece(iterator.id_);
        }

        /*
        void ResourceData::increment(
            PieceIterator & iterator)
        {
            assert(iterator.piece_);
            iterator.piece_.reset();
            if ((size_t)iterator.id_.piece + 1 < iterator.block_->piece_count()) {
                iterator.id_.inc_piece();
            } else {
                iterator.block_ = NULL;
                if ((size_t)iterator.id_.block + 1 < iterator.segment_->block_count()) {
                    iterator.id_.inc_block();
                } else {
                    iterator.segment_ = NULL;
                    if (iterator.id_.segment + 1 < end_) {
                        iterator.id_.inc_segment();
                    } else {
                        iterator.id_ = DataId(MAX_SEGMENT, 0, 0);
                        return;
                    }
                }
            }
            update(iterator);
        }
        */

        void ResourceData::increment(
            PieceIterator & iterator)
        {
            DataId & id = iterator.id_;
            iterator.piece_.reset();
            if ((size_t)id.piece + 1 < iterator.block_->piece_count()) {
                id.inc_piece();
            } else {
                iterator.block_ = NULL;
                if ((size_t)id.block + 1 < iterator.segment_->block_count()) {
                    id.inc_block();
                } else {
                    if (!iterator.segment2_->meta) {
                        assert((size_t)id.block < iterator.segment_->block_count());
                        id.inc_block();
                        return;
                    }
                    iterator.segment2_ = NULL;
                    iterator.segment_ = NULL;
                    if (id.segment + 1 < end_) {
                        id.inc_segment();
                    } else {
                        id = DataId(MAX_SEGMENT, 0, 0);
                        return;
                    }
                    iterator.segment2_ = &modify_segment2(id);
                    iterator.segment_ = iterator.segment2_->seg;
                    if (iterator.segment_ == NULL)
                        iterator.segment_ = &modify_segment(id);
                }
                iterator.block_ = &iterator.segment_->modify_block(id);
                if (iterator.segment2_->saved) {
                    data_miss.id = id;
                    data_miss.degree = 0;
                    raise(data_miss);
                }
            }
            iterator.piece_ = iterator.block_->get_piece(id);
        }

        ResourceData::lock_t ResourceData::alloc_lock(
            DataId from, 
            DataId to)
        {
            Lock * l = new Lock;
            l->from = from;
            l->to = to;
            insert_lock(l);
            return l;
        }

        void ResourceData::modify_lock(
            lock_t lock, 
            DataId from, 
            DataId to)
        {
            Lock * l = (Lock * )lock;
            locks_.erase(l);
            Lock o;
            o.from = l->from;
            o.to = l->to;
            l->from = from;
            l->to = to;
            insert_lock(l);
            remove_lock(&o);
        }

        void ResourceData::release_lock(
            lock_t & lock)
        {
            Lock * l = (Lock * )lock;
            lock = NULL;
            locks_.erase(l);
            remove_lock(l);
            delete l;
        }

        void ResourceData::query_lock(
            lock_t  lock, 
            DataId & from, 
            DataId & to)
        {
            Lock * l = (Lock * )lock;
            from = l->from;
            to = l->to;
        }

        void ResourceData::dump_locks() const
        {
            for (Lock const * p = locks_.first(); p; p = locks_.next(p)) {
                LOG_INFO("[dump_locks] lock: " << (void *)p << " from: " << p->from << " to: " << p->to);
            }
        }

        void ResourceData::insert_lock(
            Lock * l)
        {
            LOG_TRACE("[insert_lock] from: " << l->from << " to: " << l->to);
            locks_.insert(l);
        }

        void ResourceData::remove_lock(
            Lock * l)
        {
            LOG_TRACE("[remove_lock] from: " << l->from << " to: " << l->to);
            if (l->from == l->to) return;
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
            release(l->from, l->to);
        }

        void ResourceData::set_meta(
            ResourceMeta const & meta)
        {
            end_ = meta.segcount;
            meta_dirty_ = true;
        }

        void ResourceData::set_segments(
            std::vector<SegmentMeta> const & metas)
        {
            if (end_)
                return;
            DataId id(0);
            end_ = metas.size();
            meta_ready_ = 0;
            for (size_t i = 0; i < metas.size(); ++i) {
                if (metas[i].bytesize != 0) {
                    Segment2 & segment(modify_segment2(id));
                    assert(segment.meta == NULL);
                    assert(segment.seg == NULL);
                    segment.meta = new SegmentMeta(metas[i]);
                    ++meta_ready_;
                }
                id.inc_segment();
            }
            meta_dirty_ = true;
        }

        void ResourceData::set_segment_meta(
            DataId id, 
            SegmentMeta const & meta)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta == NULL) {
                segment.meta = new SegmentMeta(meta);
                if (segment.seg)
                    segment.seg->set_size(meta.bytesize);
                seg_meta_ready.id = id;
                seg_meta_ready.meta = segment.meta;
                raise(seg_meta_ready);
                if (id == data_ready.id) {
                    priv_seek(id);
                    if (id.block_piece) {
                        data_ready.id = id;
                        raise(data_ready);
                    }
                }
                ++meta_ready_;
                meta_dirty_ = true;
            } else {
                boost::system::error_code ec = segment.meta->merge(meta);
                if (ec) {
                    segment_error.id = id;
                    segment_error.meta = &meta;
                    segment_error.ec = ec;
                    raise(segment_error);
                }
            }
        }

        void ResourceData::set_segment_status(
            DataId id, 
            bool saved)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta) {
                segment.saved = saved;
            }
        }

        void ResourceData::seek(
            DataId & id)
        {
            //data_seek.id = id;
            priv_seek(id);
            //raise(data_seek);
            data_ready.id = id;
            //if (data_ready.id != data_seek.id)
            //    raise(data_ready);
        }

        void ResourceData::priv_seek(
            DataId & id)
        {
            boost::uint64_t next = id.top_segment;
            segment_map_t::const_iterator iter = 
                segments_.find(next);
            while (next < end_ 
                && iter != segments_.end()
                && iter->id == next 
                && iter->meta 
                && (iter->saved 
                    || (iter->seg 
                        && (iter->seg->full()
                            || iter->seg->seek(id))))) {
                    ++next;
                    ++iter;
                    id.block_piece = 0;
            }
            if (next >= end_) {
                id.top_segment = MAX_SEGMENT;
            } else {
                id.top_segment = next;
            }
        }

        bool ResourceData::set_piece(
            DataId id, 
            Piece::pointer piece)
        {
            Segment & segment(modify_segment(id));
            bool result = segment.set_piece(id, piece);
            if (result) {
                if (id == data_ready.id && get_segment_meta(id)) {
                    if (segment.seek(id)) {
                        if (segment.full()) {
                            segment_full.id = id;
                            raise(segment_full);
                        }
                        id.inc_segment();
                    }
                    priv_seek(id); // will change data_ready.id
                    data_ready.id = id;
                    raise(data_ready);
                }
            }
            return result;
        }

        Block const * ResourceData::map_block(
            DataId id, 
            boost::filesystem::path const & path)
        {
            return modify_segment(id).map_block(id, path);
        }

        Segment2 const * ResourceData::get_segment2(
            DataId id) const
        {
            boost::uint64_t index = id.segment;
            segment_map_t::const_iterator iter = 
                segments_.find(index);
            if (iter == segments_.end()) {
                return NULL;
            }
            return &*iter;
        }

        Segment2 & ResourceData::modify_segment2(
            DataId id)
        {
            boost::uint64_t index = id.segment;
            if (index < end_) {
                segment_map_t::iterator iter = 
                    segments_.find(index);
                if (iter == segments_.end()) {
                    iter = segments_.insert(new Segment2(index)).first;
                }
                return const_cast<Segment2 &>(*iter);
            } else {
                assert(false);
                static Segment2 empty_segment(0);
                return empty_segment;
            }
        }

        Segment const * ResourceData::get_segment(
            DataId id) const
        {
            Segment2 const * segment(get_segment2(id));
            if (segment == NULL || segment->meta == NULL) {
                return NULL;
            }
            return segment->seg;
        }

        SegmentMeta const * ResourceData::get_segment_meta(
            DataId id) const
        {
            Segment2 const * segment(get_segment2(id));
            if (segment == NULL) {
                return NULL;
            }
            return segment->meta;
        }

        Segment2 const * ResourceData::prepare_segment(
            DataId id)
        {
            boost::uint64_t index = id.segment;
            if (index >= end_) {
                return NULL;
            }
            Segment2 & segment(modify_segment2(id));
            if (segment.seg == NULL) {
                segment.seg = Segment::alloc(segment.meta ? segment.meta->bytesize : BLOCK_SIZE * 2);
            }
            return &segment;
        }

        Segment & ResourceData::modify_segment(
            DataId id)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.seg == NULL) {
                segment.seg = Segment::alloc(segment.meta ? segment.meta->bytesize : BLOCK_SIZE * 2);
            }
            return *segment.seg;
        }

        void ResourceData::release(
            DataId from,
            DataId to)
        {
            LOG_TRACE("[release] from: " << from << " to: " << to);
            boost::uint64_t segf = from.top_segment;
            boost::uint64_t segt = to.top_segment;
            segment_map_t::iterator iter = segments_.lower_bound(segf);
            if (iter == segments_.end())
                return;
            if (segf < segt && from.block_piece) {
                if (iter->id == segf++) {
                    if (iter->seg)
                        iter->seg->release(from, DataId(segf, 0, 0));
                    ++iter;
                }
                from.inc_segment(); // now from is block alignment
            }
            for (; segf < segt; ++segf) {
                if (iter != segments_.end() && iter->id == segf) {
                    if (iter->seg)
                        Segment::free(const_cast<Segment *&>(iter->seg));
                    ++iter;
                }
            }
            if (to.block_piece && iter != segments_.end() && iter->id == segf && iter->seg)
                iter->seg->release(from, to); // if from and to is in same segment, from and to is original input value
        }

    } // namespace client
} // namespace trip
