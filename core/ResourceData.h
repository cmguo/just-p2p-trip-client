// ResourceData.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_DATA_H_
#define _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

#include "trip/client/core/Segment.h"
#include "trip/client/core/ResourceEvent.h"
#include "trip/client/core/PieceIterator.h"

#include <util/event/Observable.h>

#include <framework/container/Ordered.h>
#include <framework/memory/MemoryPool.h>

namespace trip
{
    namespace client
    {

        struct ResourceMeta;

        class ResourceData
            : public util::event::Observable
        {
        public:
            ResourceData();

            ~ResourceData();

        public:
            SegmentMetaEvent seg_meta_ready;

            ResourceDataEvent data_ready;

            ResourceDataEvent data_miss;

            //ResourceDataEvent data_seek;

            ResourceDataEvent segment_full;

            SegmentErrorEvent segment_error;

            static framework::memory::MemoryPool & mpool();

        public:
            boost::uint64_t get_segment_count() const
            {
                return end_;
            }

            bool segments_ready() const
            {
                return end_ == meta_ready_;
            }

            void get_segments(
                std::vector<SegmentMeta> & metas) const;

            SegmentMeta const * get_segment_meta(
                DataId id) const;

            Segment const * get_segment(
                DataId id) const;

            Piece::pointer get_piece(
                DataId id, 
                size_t degree); // Emergency degree

            void get_segment_map(
                DataId from, 
                boost::uint16_t count, 
                boost::dynamic_bitset<boost::uint32_t> & map) const;

            void get_block_map(
                DataId id, 
                boost::uint16_t count, 
                boost::dynamic_bitset<boost::uint32_t> & map) const;

            bool meta_dirty() const;

        public:
            PieceIterator iterator_at(
                DataId id, 
                bool update = true);

            void update(
                PieceIterator & iterator);

            void increment(
                PieceIterator & iterator);

        public:
            typedef void const * lock_t;

            lock_t alloc_lock(
                DataId from, 
                DataId to);

            void modify_lock(
                lock_t lock, 
                DataId from, 
                DataId to);

            void release_lock(
                lock_t & lock);

            void query_lock(
                lock_t lock, 
                DataId & from, 
                DataId & to);

            void dump_locks() const;

        public:
            void set_segments(
                std::vector<SegmentMeta> const & metas);

            void set_segment_meta(
                DataId id, 
                SegmentMeta const & meta);

            void set_segment_status(
                DataId id, 
                bool saved);

            void seek(
                DataId & id);

            bool set_piece(
                DataId id, 
                Piece::pointer piece);

            Block const * map_block(
                DataId id, 
                boost::filesystem::path const & path);

        public:
            Segment2 const * prepare_segment(
                DataId id);

            Segment2 const * get_segment2(
                DataId segid) const;

        protected:
            void set_meta(
                ResourceMeta const & meta);

        private:
            void priv_seek(
                DataId & id);

            Segment & modify_segment(
                DataId id);

            Segment2 & modify_segment2(
                DataId id);

            void release(
                DataId from, 
                DataId to);

        private:
            typedef framework::container::Ordered<
                Segment2, 
                Segment2::idkey
            > segment_map_t;

        private:
            struct Lock
                : framework::container::OrderedHook<Lock>::type
            {
                DataId from;
                DataId to;
                friend bool operator<(
                    Lock const & l, 
                    Lock const & r)
                {
                    return l.from < r.from;
                }
            };

            typedef framework::container::Ordered<Lock, 
                framework::container::identity<Lock>, 
                std::less<Lock>, 
                framework::container::ordered_non_unique_tag
            > locks_map_t; 

            void insert_lock(
                Lock * l);

            void remove_lock(
                Lock * l);

            locks_map_t & locks()
            {
                return locks_;
            }

        private:
            template <typename Archive>
            friend void serialize(
                Archive & ar, 
                ResourceData & t);

            boost::uint64_t next_;
            boost::uint64_t end_;
            segment_map_t segments_;
            locks_map_t locks_;
            mutable bool meta_dirty_;
            boost::uint64_t meta_ready_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

