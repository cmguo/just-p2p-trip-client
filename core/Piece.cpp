// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Piece.h"
#include "trip/client/core/PieceIterator.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/BlockData.h"
#include "trip/client/core/PartPiece.h"
#include "trip/client/core/ResourceData.h"

namespace trip
{
    namespace client
    {

        void PieceIterator::increment()
        {
            resource_->increment(*this);
        }

        typedef void (*free_piece_t)(Piece *);

        static free_piece_t free_piece[] = {
            (free_piece_t)PoolPiece::free, 
            (free_piece_t)BlockPiece::free, 
            (free_piece_t)PartPiece::free, 
        };

        void Piece::free(
            Piece * p)
        {
            free_piece[p->type_](p);
        }

    } // namespace client
} // namespace trip
