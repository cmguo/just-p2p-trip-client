// PoolPiece.h

#ifndef _TRIP_CLIENT_CORE_POOL_PIECE_H_
#define _TRIP_CLIENT_CORE_POOL_PIECE_H_

#include "trip/client/core/Piece.h"

#include <framework/memory/MemoryPool.h>

namespace trip
{
    namespace client
    {

        class PoolPiece
            : public Piece
        {
        public:
            static void set_capacity(
                size_t capacity);

        public:
            static PoolPiece * alloc(
                boost::uint16_t size = PIECE_SIZE);

            static void free(
                PoolPiece * p);

            static framework::memory::MemoryPool & mpool();

        private:
            PoolPiece(
                boost::uint16_t size)
                : Piece(0, buf, size)
            {
            }

            ~PoolPiece() {}

        private:
            boost::uint8_t buf[PIECE_SIZE];
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_POOL_PIECE_H_
