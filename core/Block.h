// Block.h

#ifndef _TRIP_CLIENT_CORE_BLOCK_H_
#define _TRIP_CLIENT_CORE_BLOCK_H_

#include "trip/client/core/Piece.h"

#include <framework/container/Array.h>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

namespace trip
{
    namespace client
    {

        class BlockData;

        class Block
        {
        public:
            static Block * alloc(
                boost::uint32_t size = 0);

            static void free(
                Block * p);

        public:
            Block(
                boost::uint32_t size = 0);

        public:
            bool full() const
            {
                return left_ == 0;
            }

            typedef framework::container::Array<Piece::pointer const> piece_array_t;

            piece_array_t pieces() const
            {
                return framework::container::make_array(pieces_, piece_count_);
            }

            boost::uint16_t piece_count() const
            {
                return piece_count_;
            }

            boost::uint32_t get_size() const;

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

            bool set_data(
                boost::intrusive_ptr<BlockData> data);

            void release(
                DataId from, 
                DataId to);

        private:
            boost::uint16_t left_;
            mutable boost::uint16_t touched_;
            boost::uint16_t piece_count_;
            boost::uint16_t last_piece_size_;
            Piece::pointer pieces_[PIECE_PER_BLOCK];
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_BLOCK_H_

