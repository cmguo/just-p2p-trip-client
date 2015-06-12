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
            bool full() const
            {
                return left_ == 0;
            }

            std::vector<Piece::pointer> const & pieces() const
            {
                return pieces_;
            }

            Piece::pointer get_piece(
                DataId id) const;

            bool touched() const;

            void get_stat(
                boost::dynamic_bitset<boost::uint32_t> & map) const;

        public:
            void set_size(
                boost::uint32_t size);

            bool seek(
                DataId & id);

            bool set_piece(
                DataId id, 
                Piece::pointer piece);

            void release(
                DataId from, 
                DataId to);

        private:
            boost::uint16_t left_;
            mutable boost::uint16_t touched_;
            boost::uint16_t piece_count_;
            boost::uint16_t last_piece_size_;
            std::vector<Piece::pointer> pieces_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_BLOCK_H_

