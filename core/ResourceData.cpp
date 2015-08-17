// ResourceData.cpp

#include "trip/client/Common.h"
#include "trip/client/core/ResourceData.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.ResourceData", framework::logger::Debug);

        ResourceData::ResourceData()
            : end_(0)
        {
        }

        ResourceData::~ResourceData()
        {
        }

        void ResourceData::set_meta(
            ResourceMeta const & meta)
        {
            end_ = meta.duration / meta.interval;
        }

        Piece::pointer ResourceData::get_piece(
            DataId id)
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
                raise(data_miss);
                if (segment2->seg) {
                    p = segment2->seg->get_piece(id);
                }
            }
            return p;
        }

        void ResourceData::get_stat(
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
        }

        PieceIterator ResourceData::iterator_at(
            DataId id)
        {
            return PieceIterator(*this, id);
        }

        void ResourceData::update(
            PieceIterator & iterator)
        {
            DataId id = iterator.id();
            if (!iterator.block_) {
                if (!iterator.segment_) {
                    iterator.segment_ = get_segment(id);
                }
                if (iterator.segment_) {
                    iterator.block_ = iterator.segment_->get_block(id);
                }
            }
            if (iterator.block_) {
                iterator.piece_ = iterator.block_->get_piece(id);
            }
            if (!iterator.piece_) {
                Segment2 const * segment2 = get_segment2(id);
                if (segment2 && segment2->saved) {
                    data_miss.id = id;
                    raise(data_miss);
                    update(iterator);
                }
            }
        }

        void ResourceData::increment(
            PieceIterator & iterator)
        {
            assert(iterator.piece_);
            iterator.piece_.reset();
            if ((size_t)iterator.id_.piece + 1 < iterator.block_->pieces().size()) {
                iterator.id_.inc_piece();
            } else {
                iterator.block_ = NULL;
                if ((size_t)iterator.id_.block + 1 < iterator.segment_->blocks().size()) {
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
            lock_t lock)
        {
            Lock * l = (Lock * )lock;
            locks_.erase(l);
            remove_lock(l);
            delete l;
        }

        void ResourceData::insert_lock(
            Lock * l)
        {
            LOG_INFO("[insert_lock] from: " << l->from << " to: " << l->to);
            locks_.insert(l);
        }

        void ResourceData::remove_lock(
            Lock * l)
        {
            LOG_INFO("[remove_lock] from: " << l->from << " to: " << l->to);
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

        bool ResourceData::save_segment(
            DataId id, 
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
            DataId id, 
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
            }
        }

        bool ResourceData::load_block_stat(
            DataId id, 
            boost::dynamic_bitset<boost::uint32_t> & map)
        {
            modify_segment(id).load_block_stat(id, map);
            return true;
        }

        void ResourceData::seek(
            DataId & id)
        {
            data_seek.id = id;
            priv_seek(id);
            raise(data_seek);
            data_ready.id = id;
            if (data_ready.id != data_seek.id)
                raise(data_ready);
        }

        void ResourceData::priv_seek(
            DataId & id)
        {
            boost::uint64_t next = id.top_segment;
            std::map<boost::uint64_t, Segment2>::const_iterator iter = 
                segments_.find(next);
            if (next < end_) {
                if (iter != segments_.end() && iter->second.meta && iter->second.seg) {
                    if (!iter->second.seg->seek(id)) {
                        return;
                    }
                    ++next;
                    ++iter;
                }
            }
            while (next < end_ 
                && iter != segments_.end()
                && iter->first == next 
                && iter->second.meta
                && (iter->second.saved || (iter->second.seg && iter->second.seg->full()))) {
                    ++next;
                    ++iter;
            }
            id.block_piece = 0;
            if (next >= end_) {
                id.top_segment = MAX_SEGMENT;
            } else {
                id.top_segment = next;
                if (iter != segments_.end() && iter->first == next && iter->second.meta && iter->second.seg)
                    iter->second.seg->seek(id);
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

        ResourceData::Segment2 const * ResourceData::get_segment2(
            DataId id) const
        {
            boost::uint64_t index = id.segment;
            std::map<boost::uint64_t, Segment2>::const_iterator iter = 
                segments_.find(index);
            if (iter == segments_.end()) {
                return NULL;
            }
            return &iter->second;
        }

        ResourceData::Segment2 & ResourceData::modify_segment2(
            DataId id)
        {
            boost::uint64_t index = id.segment;
            if (index < end_) {
                std::map<boost::uint64_t, Segment2>::iterator iter = 
                    segments_.find(index);
                if (iter == segments_.end()) {
                    iter = segments_.insert(std::make_pair(index, Segment2())).first;
                }
                return iter->second;
            } else {
                assert(false);
                static Segment2 empty_segment;
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

        ResourceData::Segment2 const * ResourceData::prepare_segment(
            DataId id)
        {
            boost::uint64_t index = id.segment;
            if (index >= end_) {
                return NULL;
            }
            Segment2 & segment(modify_segment2(id));
            if (segment.seg == NULL) {
                segment.seg = new Segment(segment.meta ? segment.meta->bytesize : BLOCK_SIZE * 2);
            }
            return &segment;
        }

        Segment & ResourceData::modify_segment(
            DataId id)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.seg == NULL) {
                segment.seg = new Segment(segment.meta ? segment.meta->bytesize : BLOCK_SIZE * 2);
            }
            return *segment.seg;
        }

        void ResourceData::release(
            DataId from,
            DataId to)
        {
            LOG_INFO("[release] from: " << from << " to: " << to);
            boost::uint64_t segf = from.top_segment;
            boost::uint64_t segt = to.top_segment;
            std::map<boost::uint64_t, Segment2>::iterator iter = segments_.lower_bound(segf);
            if (iter == segments_.end())
                return;
            if (segf < segt && from.block_piece) {
                if (iter->first == segf++ && iter->second.seg) {
                    iter->second.seg->release(from, DataId(segf, 0, 0));
                    ++iter;
                }
            }
            for (; segf < segt; ++segf) {
                if (iter != segments_.end() && iter->first == segf && iter->second.seg) {
                    delete iter->second.seg;
                    iter->second.seg = NULL;
                    ++iter;
                }
            }
            if (to.block_piece && iter != segments_.end() && iter->first == segf && iter->second.seg)
                iter->second.seg->release(DataId(segf, 0, 0), to);
        }

    } // namespace client
} // namespace trip
