// Block.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Block.h"

namespace trip
{
    namespace client
    {

        Block::Block(
            boost::uint16_t npiece)
            : next_(0)
            , expire_(0)
        {
            pieces_.resize(npiece);
        }

        Block::Block(
            void * map_addr, 
            boost::uint32_t size)
        {
            boost::uint16_t npiece = (size + PIECE_SIZE - 1) / PIECE_SIZE;
            pieces_.resize(npiece);
            boost::uint8_t * addr = (boost::uint8_t *)map_addr;
            for (size_t i = 0; i < pieces_.size(); ++i) {
                Piece * p = Piece::alloc();
                p->data = addr;
                p->size = PIECE_SIZE;
                addr += PIECE_SIZE;
                pieces_[i] = p;
            }
            pieces_[npiece - 1]->size = size % PIECE_SIZE;
        }

        Piece::pointer Block::get_piece(
            boost::uint64_t id) const
        {
            boost::uint16_t index = PIECE(id);
            assert(index < pieces_.size());
            expire_ = 0;
            if (index < pieces_.size())
                return pieces_[index];
            else
                return NULL;
        }

        bool Block::expired(
            boost::uint16_t expire) const
        {
            if (expire_ < expire)
                return false;
            for (size_t i = 0; i < pieces_.size(); ++i) {
                if (pieces_[i]->nref > 1)
                    return false;
            }
            return true;
        }

        boost::uint64_t Block::set_piece(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            boost::uint16_t index = PIECE(id);
            assert(index < pieces_.size());
            assert(pieces_[index] == NULL);
            if (index < pieces_.size() && pieces_[index] == NULL) {
                pieces_[index] = piece;
                if (next_ == index) {
                    ++next_;
                    while (next_ < pieces_.size() && pieces_[next_])
                        ++next_;
                    if (next_ == pieces_.size()) {
                        next_ = 0xfff;
                    }
                }
            }
            return next_;
        }
      

    } // namespace client
} // namespace trip

