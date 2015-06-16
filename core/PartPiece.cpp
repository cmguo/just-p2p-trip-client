// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/PartPiece.h"

namespace trip
{
    namespace client
    {

        PartPiece * PartPiece::alloc(
            Piece::pointer piece, 
            boost::uint16_t offset, 
            boost::uint16_t size)
        {
            return new PartPiece(piece, offset, size);
        }

        void PartPiece::free(
            PartPiece * p)
        {
            delete p;
        }

    } // namespace client
} // namespace trip
