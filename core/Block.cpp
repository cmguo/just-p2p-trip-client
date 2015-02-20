// Block.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Block.h"
#include "trip/client/core/BlockData.h"

namespace trip
{
    namespace client
    {

        Block::Block(
            boost::uint32_t size)
            : next_(0)
            , expire_(0)
        {
            set_size(size);
        }

        Block::Block(
            BlockData * data)
        {
            set_size(data->size_);
            boost::uint8_t * addr = (boost::uint8_t *)data->map_addr_;
            for (size_t i = 0; i < pieces_.size() - 1; ++i) {
                pieces_[i] = BlockPiece::alloc(data, addr, PIECE_SIZE);
                addr += PIECE_SIZE;
            }
            pieces_[piece_count_ - 1] = BlockPiece::alloc(data, addr, last_piece_size_);
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
            return (expire < expire_);
        }

        void Block::get_stat(
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
            for (size_t i = 0; i < pieces_.size() - 1; ++i) {
                map.push_back(pieces_[i]);
            }
        }

        void Block::set_size(
            boost::uint32_t size)
        {
            piece_count_ = boost::uint16_t(size / PIECE_SIZE);
            last_piece_size_ = boost::uint16_t(size % PIECE_SIZE);
            if (last_piece_size_) {
                ++piece_count_;
            } else {
                last_piece_size_ = PIECE_SIZE;
            }
            pieces_.resize(piece_count_);
        }

        boost::uint64_t Block::set_piece(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            boost::uint16_t index = PIECE(id);
            assert(index < pieces_.size());
            assert(pieces_[index] == NULL);
            if (index < pieces_.size() && pieces_[index] == NULL) {
                pieces_[index].swap(piece);
                if (next_ == index) {
                    ++next_;
                    while (next_ < pieces_.size() && pieces_[next_])
                        ++next_;
                    if (next_ == pieces_.size()) {
                        next_ = MAX_PIECE;
                    }
                }
            }
            return next_;
        }
      
        void Block::release(
            boost::uint64_t from,
            boost::uint64_t to)
        {
            boost::uint64_t picf = BLOCK(from);
            boost::uint64_t pict = BLOCK(to);
            while (picf < pict) {
                pieces_[picf].reset();
                ++picf;
            } 
        }

    } // namespace client
} // namespace trip

