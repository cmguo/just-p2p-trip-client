// Block.cpp {

#include "trip/client/Common.h"
#include "trip/client/core/Block.h"
#include "trip/client/core/BlockData.h"

namespace trip
{
    namespace client
    {

        Block::Block(
            boost::uint32_t size)
            : left_(0)
            , touched_(false)
        {
            set_size(size);
        }

        boost::uint32_t  Block::get_size() const
        {
            return PIECE_SIZE * piece_count_ + last_piece_size_ - PIECE_SIZE;
        }

        Piece::pointer Block::get_piece(
            DataId id) const
        {
            boost::uint16_t index = id.piece;
            assert(index < piece_count_);
            if (index < piece_count_) {
                touched_ = true;
                return pieces_[index];
            } else {
                return NULL;
            }
        }

        bool Block::touched() const
        {
            bool t = touched_;
            touched_ = false;
            return t;
        }

        void Block::get_stat(
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
            for (boost::uint16_t i = 0; i < piece_count_ - 1; ++i) {
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
            left_ = 0;
            for (boost::uint16_t i = 0; i < piece_count_; ++i) {
                if (!pieces_[i])
                    ++left_;
            }
        }

        bool Block::seek(
            DataId & id)
        {
            boost::uint16_t next = id.piece;
            while (next < piece_count_ && pieces_[next])
                ++next;
            if (next >= piece_count_) {
                id.piece = 0;
                return true;
            } else {
                id.piece = next;
                return false;
            }
        }

        bool Block::set_piece(
            DataId id, 
            Piece::pointer piece)
        {
            boost::uint16_t index = id.piece;
            assert(index < piece_count_);
            assert(pieces_[index] == NULL);
            if (index < piece_count_ && pieces_[index] == NULL) {
                pieces_[index].swap(piece);
                --left_;
            }
            return !piece;
        }
      
        bool Block::set_data(
            boost::intrusive_ptr<BlockData> data)
        {
            assert(get_size() == data->size_);
            boost::uint8_t * addr = (boost::uint8_t *)data->map_addr_;
            for (boost::uint16_t i = 0; i < piece_count_ - 1; ++i) {
                pieces_[i] = BlockPiece::alloc(data, addr, PIECE_SIZE);
                addr += PIECE_SIZE;
            }
            pieces_[piece_count_ - 1] = BlockPiece::alloc(data, addr, last_piece_size_);
            return true;
        }

        void Block::release(
            DataId from,
            DataId to)
        {
            boost::uint32_t picf = from.piece;
            boost::uint32_t pict = to.piece ? to.piece : piece_count_;
            while (picf < pict) {
                pieces_[picf].reset();
                ++picf;
                ++left_;
            } 
        }

    } // namespace client
} // namespace trip

