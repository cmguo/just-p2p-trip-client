// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Piece.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/BlockData.h"

namespace trip
{
    namespace client
    {

        void Piece::free(
            Piece * p)
        {
            if (p->data_ == (boost::uint8_t *)(p + 1))
                PoolPiece::free((PoolPiece *)p);
            else
                BlockPiece::free((BlockPiece *)p);
        }

    } // namespace client
} // namespace trip
