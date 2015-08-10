// DataId.h

#ifndef _TRIP_CLIENT_CORE_DATA_ID_H_
#define _TRIP_CLIENT_CORE_DATA_ID_H_

#include <boost/operators.hpp>

namespace trip
{
    namespace client
    {

#define BIT_COUNT 63
#define PIECE_BIT 6
#define BLOCK_BIT 6
#define SEGMENT_BIT (BIT_COUNT - PIECE_BIT - BLOCK_BIT)

#define MAX_PIECE boost::uint16_t(1 << PIECE_BIT)
#define MAX_BLOCK boost::uint16_t(1 << BLOCK_BIT)
#define MAX_SEGMENT boost::uint64_t(1ULL << SEGMENT_BIT)

#define MAKE_ID(s, b, p) (((s) << (PIECE_BIT + BLOCK_BIT)) | ((b) << PIECE_BIT) | (p))

        struct DataId
            : boost::totally_ordered<DataId>
        {
            union {
#ifdef BOOST_BIG_ENDIAN
                struct {
                    boost::uint64_t top : 1;
                    boost::uint64_t segment : SEGMENT_BIT;
                    boost::uint64_t block : BLOCK_BIT;
                    boost::uint64_t piece : PIECE_BIT;
                };
                struct {
                    boost::uint64_t top_segment : 1 + SEGMENT_BIT;
                    boost::uint64_t block_piece : BLOCK_BIT + PIECE_BIT;
                };
                struct {
                    boost::uint64_t top_segment_block : 1 + SEGMENT_BIT + BLOCK_BIT;
                    boost::uint64_t piece2 : PIECE_BIT;
                };
#else
                struct {
                    boost::uint64_t piece : PIECE_BIT;
                    boost::uint64_t block : BLOCK_BIT;
                    boost::uint64_t segment : SEGMENT_BIT;
                    boost::uint64_t top : 1;
                };
                struct {
                    boost::uint64_t block_piece : BLOCK_BIT + PIECE_BIT;
                    boost::uint64_t top_segment : 1 + SEGMENT_BIT;
                };
                struct {
                    boost::uint64_t piece2 : PIECE_BIT;
                    boost::uint64_t top_segment_block : 1 + SEGMENT_BIT + BLOCK_BIT;
                };
#endif
                boost::uint64_t id;
            };

            DataId(
                boost::uint64_t id = 0)
            {
                this->id = id;
            }

            DataId(
                boost::uint64_t segment, 
                boost::uint32_t block, 
                boost::uint32_t piece)
            {
                this->id = MAKE_ID(segment, block, piece);
            }

            void inc_piece(
                boost::uint32_t n = 1)
            {
                assert(piece + n <= MAX_PIECE);
                id += n;
            }

            void inc_block(
                boost::uint32_t n = 1)
            {
                assert(block + n <= MAX_BLOCK);
                piece = 0;
                id += (n << PIECE_BIT);
            }

            void inc_block_piece(
                boost::uint32_t n = 1)
            {
                assert(block_piece + n <= (1 << (PIECE_BIT + BLOCK_BIT)));
                id += n;
            }

            void inc_segment(
                boost::uint64_t n = 1)
            {
                assert(segment + n <= MAX_SEGMENT);
                block_piece = 0;
                id += (n << (PIECE_BIT + BLOCK_BIT));
            }

            friend bool operator<(
                DataId const & l, 
                DataId const & r)
            {
                return l.id < r.id;
            }

            friend bool operator==(
                DataId const & l, 
                DataId const & r)
            {
                return l.id == r.id;
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_DATA_ID_H_
