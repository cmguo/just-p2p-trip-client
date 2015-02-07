// PoolPiece.h

#ifndef _TRIP_CLIENT_CORE_POOL_PIECE_H_
#define _TRIP_CLIENT_CORE_POOL_PIECE_H_

#include "trip/client/core/Piece.h"

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

            static PoolPiece * alloc(
                boost::uint16_t size);

            void set_size(
                boost::uint16_t size)
            {
                Piece::set_size(size);
            }

        private:
            friend class Piece;

            static void free(
                PoolPiece * p);

        private:
            PoolPiece(
                boost::uint16_t size)
                : Piece(buf, size)
            {
            }

            ~PoolPiece() {}

        private:
            boost::uint8_t buf[PIECE_SIZE];
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_POOL_PIECE_H_