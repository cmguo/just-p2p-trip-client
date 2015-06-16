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
            bool full() const
            {
                return left_ == 0;
            }

            boost::uint32_t size() const;

            boost::uint32_t piece_count() const;

            std::vector<Block *> const & blocks() const
            {
                return blocks_;
            }

            Block const * get_block(
                DataId id) const;

            Piece::pointer get_piece(
                DataId id) const;

            void get_stat(
                boost::dynamic_bitset<boost::uint32_t> & map) const;

        public:
            md5_bytes cache_md5sum() const;

            static md5_bytes file_md5sum(
                boost::filesystem::path const & path);

            bool save(
                boost::filesystem::path const & path) const;

            bool load(
                boost::filesystem::path const & path);

        public:
            void set_size(
                boost::uint32_t size);

            bool load_block_stat(
                DataId id, 
                boost::dynamic_bitset<boost::uint32_t> & map);

            bool seek(
                DataId & id);

            bool set_piece(
                DataId id, 
                Piece::pointer piece);

            Block const * map_block(
                DataId id, 
                boost::filesystem::path const & path);

            void release(
                DataId from, 
                DataId to);

        private:
            Block & modify_block(
                DataId id);

        private:
            boost::uint16_t left_;
            boost::uint16_t block_count_;
            boost::uint32_t last_block_size_;
            std::vector<Block *> blocks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SEGMENT_H_

