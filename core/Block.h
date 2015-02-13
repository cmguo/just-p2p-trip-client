// Block.h

#ifndef _TRIP_CLIENT_CORE_BLOCK_H_
#define _TRIP_CLIENT_CORE_BLOCK_H_

#include "trip/client/core/Piece.h"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

namespace trip
{
    namespace client
    {

        class BlockData;

        class Block
        {
        public:
            Block(
                boost::uint32_t size = 0);

            Block(
                BlockData * data);

        public:
            boost::uint16_t next() const
            {
                return next_;
            }

            bool finished() const
            {
                return next_ == MAX_PIECE;
            }

            std::vector<Piece::pointer> const & pieces() const
            {
                return pieces_;
            }

            Piece::pointer get_piece(
                boost::uint64_t id) const;

            bool expired(
                boost::uint16_t expire) const;

            void get_stat(
                boost::dynamic_bitset<boost::uint32_t> & map) const;

        public:
            boost::uint64_t set_piece(
                boost::uint64_t id, 
                Piece::pointer piece);

            void release(
                boost::uint64_t from, 
                boost::uint64_t to);

        private:
            boost::uint16_t next_;
            mutable boost::uint16_t expire_;
            boost::uint16_t piece_count_;
            boost::uint16_t last_piece_size_;
            std::vector<Piece::pointer> pieces_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_BLOCK_H_

