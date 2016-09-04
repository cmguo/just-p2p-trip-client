// PartPiece.h

#ifndef _TRIP_CLIENT_CORE_PART_PIECE_H_
#define _TRIP_CLIENT_CORE_PART_PIECE_H_

#include "trip/client/core/Piece.h"

namespace trip
{
    namespace client
    {

        class PartPiece
            : public Piece
        {
        public:
            static PartPiece * alloc(
                Piece::pointer piece, 
                boost::uint16_t offset, 
                boost::uint16_t size = 0);

            static void free(
                PartPiece * p);

        private:
            PartPiece(
                Piece::pointer piece, 
                boost::uint16_t offset, 
                boost::uint16_t size)
                : Piece(2, (boost::uint8_t *)piece->data() + offset, size)
                , piece_(piece)
            {
                assert(offset < piece->size());
                if (size == 0)
                    set_size(piece->size() - offset);
                else
                    assert(offset + size <= piece->size());
            }

            ~PartPiece() {}

        private:
            Piece::pointer piece_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_PART_PIECE_H_
