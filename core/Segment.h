// Segment.h

#ifndef _TRIP_CLIENT_CORE_SEGMENT_H_
#define _TRIP_CLIENT_CORE_SEGMENT_H_

#include "trip/client/core/Block.h"

#include <framework/string/Md5.h>
#include <framework/container/Array.h>
#include <framework/container/Ordered.h>

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        using framework::string::Md5Sum;

        struct SegmentMeta
        {
            boost::uint32_t duration;
            boost::uint32_t bytesize;
            Md5Sum md5sum;

            SegmentMeta()
                : duration(0)
                  , bytesize(0)
            {
            }
        };

        class Segment
        {
        public:
            static Segment * alloc(
                boost::uint32_t size = 0);

            static void free(
                Segment *& p);

        public:
            Segment(
                boost::uint32_t size = 0);

            ~Segment();

        public:
            bool full() const
            {
                return left_ == 0;
            }

            boost::uint32_t size() const;

            boost::uint32_t piece_count() const;

            boost::uint16_t block_count() const
            {
                return block_count_;
            }

            typedef framework::container::Array<Block * const> block_array_t;

            block_array_t blocks() const
            {
                return framework::container::make_array(blocks_, block_count_);
            }

            Block const * get_block(
                DataId id) const;

            Piece::pointer get_piece(
                DataId id) const;

            void get_status(
                DataId from, 
                boost::dynamic_bitset<boost::uint32_t> & map) const;

        public:
            Md5Sum cache_md5sum() const;

            static Md5Sum file_md5sum(
                boost::filesystem::path const & path);

            bool save(
                boost::filesystem::path const & path, 
                boost::system::error_code & ec) const;

        public:
            void set_size(
                boost::uint32_t size);

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
            friend class ResourceData;
            Block & modify_block(
                DataId id);

        private:
            template <typename Archive>
            friend void serialize(
                Archive & ar, 
                Segment & t);

        private:
            boost::uint16_t left_;
            boost::uint16_t block_count_;
            boost::uint32_t last_block_size_;
            Block * blocks_[BLOCK_PER_SEGMENT];
        };

        struct Segment2
            : framework::container::OrderedHook<Segment2>::type
        {
            struct idkey
            {
                typedef boost::uint64_t value_type;
                typedef boost::uint64_t result_type;
                result_type operator () (
                    Segment2 const & t) const
                {
                    return t.id;
                }
            };

            union {
                struct {
                    boost::uint64_t id : SEGMENT_BIT;
                    boost::uint64_t saved : 1;
                    boost::uint64_t external : 1;
                };
                boost::uint64_t id2;
            };
            SegmentMeta * meta;
            Segment * seg;

            Segment2(
                boost::uint64_t id)
                : id(id)
                , saved(0)
                , external(0)
                , meta(NULL)
                , seg(NULL)
            {
            }

            boost::uint16_t block_count() const
            {
                return boost::uint16_t((meta->bytesize + BLOCK_SIZE - 1) / BLOCK_SIZE);
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SEGMENT_H_
