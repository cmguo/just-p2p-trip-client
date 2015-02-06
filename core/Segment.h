// Segment.h

#ifndef _TRIP_CLIENT_CORE_SEGMENT_H_
#define _TRIP_CLIENT_CORE_SEGMENT_H_

#include "trip/client/core/Block.h"

#include <framework/string/Md5.h>

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        typedef framework::string::Md5::bytes_type md5_bytes;

        struct SegmentMeta
        {
            boost::uint32_t duration;
            boost::uint32_t bytesize;
            md5_bytes md5sum;

            SegmentMeta()
                : duration(0)
                , bytesize(0)
            {
            }
        };

        class Segment
        {
        public:
            Segment(
                boost::uint32_t size = 0);

        public:
            boost::uint64_t next() const
            {
                return MAKE_ID(0, next_, blocks_[next_]->next());
            }

            bool finished() const
            {
                return next_ == MAX_BLOCK;
            }

            md5_bytes cache_md5sum() const;

            Block const & get_block(
                boost::uint64_t id) const;

            boost::uint16_t piece_size(
                boost::uint64_t id) const;

            Piece::pointer get_piece(
                boost::uint64_t id) const;

        public:
            static md5_bytes file_md5sum(
                boost::filesystem::path const & path);

            bool save(
                boost::filesystem::path const & path) const;

            bool load(
                boost::filesystem::path const & path);

        public:
            boost::uint64_t set_piece(
                boost::uint64_t id, 
                Piece::pointer piece);

            Block const * map_block(
                boost::uint64_t id, 
                boost::filesystem::path const & path);

            bool unmap_block(
                boost::uint64_t id);

        private:
            Block & modify_block(
                boost::uint64_t id);

        private:
            SegmentMeta const * meta_;
            boost::uint16_t next_;
            boost::uint32_t piece_count_;
            boost::uint16_t last_piece_size_;
            boost::uint16_t last_block_piece_;
            boost::uint32_t last_block_size_;
            std::vector<Block *> blocks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SEGMENT_H_

