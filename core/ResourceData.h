// ResourceData.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_DATA_H_
#define _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

#include "trip/client/core/Segment.h"
#include "trip/client/core/ResourceEvent.h"

#include <util/event/Observable.h>

#include <framework/container/OrderedList.h>

namespace trip
{
    namespace client
    {

        class ResourceData
            : public util::event::Observable
        {
        public:
            ResourceData();

            ~ResourceData();

        public:
            DataChangedEvent data_changed;

        public:
            Segment const & get_segment(
                boost::uint64_t segid) const;

            boost::uint16_t piece_size(
                boost::uint64_t index) const;

            Piece::pointer get_piece(
                boost::uint64_t id);

        public:
            typedef void const * lock_t;

            lock_t alloc_lock(
                boost::uint64_t from, 
                boost::uint64_t to);

            void modify_lock(
                lock_t lock, 
                boost::uint64_t from, 
                boost::uint64_t to);

            void release_lock(
                lock_t lock);

        public:
            bool save_segment(
                boost::uint64_t id, 
                boost::filesystem::path const & path);

            bool load_segment(
                boost::uint64_t id,
                boost::filesystem::path const & path);

            boost::uint64_t set_piece(
                boost::uint64_t id, 
                Piece::pointer piece);

            Block const * map_block(
                boost::uint64_t id, 
                boost::filesystem::path const & path);

        private:
            Segment & modify_segment(
                boost::uint64_t id);

        private:
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

        private:
            Segment2 & modify_segment2(
                boost::uint64_t id);

            Segment2 const & get_segment2(
                boost::uint64_t segid) const;

            void release(
                boost::uint64_t from, 
                boost::uint64_t to);

        private:
            struct Lock
                : framework::container::OrderedListHook<Lock>::type
            {
                boost::uint64_t from;
                boost::uint64_t to;
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
            framework::container::OrderedList<Lock> locks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

