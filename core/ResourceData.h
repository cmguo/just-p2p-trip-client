// ResourceData.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_DATA_H_
#define _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

#include "trip/client/core/Segment.h"
#include "trip/client/core/ResourceEvent.h"

#include <util/event/Observable.h>

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

            bool unmap_block(
                boost::uint64_t id);

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

        private:
            boost::uint64_t next_;
            boost::uint64_t end_;
            std::map<boost::uint64_t, Segment2> segments_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_DATA_H_

