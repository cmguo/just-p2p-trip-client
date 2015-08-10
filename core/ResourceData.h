// ResourceData.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_DATA_H_
#define _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

#include "trip/client/core/Segment.h"
#include "trip/client/core/ResourceEvent.h"
#include "trip/client/core/PieceIterator.h"

#include <util/event/Observable.h>

#include <framework/container/Ordered.h>

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

            DataEvent data_ready;

            DataEvent data_miss;

            DataEvent data_seek;

            DataEvent segment_full;

        public:
            SegmentMeta const * get_segment_meta(
                DataId id) const;

            Segment const * get_segment(
                DataId id) const;

            Piece::pointer get_piece(
                DataId id);

            void get_stat(
                boost::dynamic_bitset<boost::uint32_t> & map) const;

        public:
            PieceIterator iterator_at(
                DataId id);

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
                lock_t lock);

        public:
            bool save_segment(
                DataId id, 
                boost::filesystem::path const & path);

            bool load_segment(
                DataId id,
                boost::filesystem::path const & path);

        public:
            void set_segment_meta(
                DataId id, 
                SegmentMeta const & meta);

            bool load_block_stat(
                DataId id, 
                boost::dynamic_bitset<boost::uint32_t> & map);

            void seek(
                DataId & id);

            bool set_piece(
                DataId id, 
                Piece::pointer piece);

            Block const * map_block(
                DataId id, 
                boost::filesystem::path const & path);

        public:
            struct Segment2
            {
                SegmentMeta * meta;
                Segment * seg;
                bool saved;
                Segment2()
                    : meta(NULL)
                    , seg(NULL)
                    , saved(false)
                {
                }
            };

            Segment2 const * prepare_segment(
                DataId id);

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

            Segment2 const * get_segment2(
                DataId segid) const;

            void release(
                DataId from, 
                DataId to);

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

            void release_lock(
                Lock * l);

        private:
            boost::uint64_t next_;
            boost::uint64_t end_;
            std::map<boost::uint64_t, Segment2> segments_;
            framework::container::Ordered<Lock> locks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

