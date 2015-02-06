// Segment.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Segment.h"

#include <framework/filesystem/File.h>

namespace trip
{
    namespace client
    {

        Segment::Segment(
            boost::uint32_t size)
            : next_(0)
        {
            piece_count_ = size / PIECE_SIZE;
            last_piece_size_ = size % PIECE_SIZE;
            if (last_piece_size_ == 0) {
                last_piece_size_ = PIECE_SIZE;
            } else {
                ++piece_count_;
            }
            size_t block_count = piece_count_ + PIECE_PER_BLOCK;
            last_block_piece_ = piece_count_ % PIECE_PER_BLOCK;
            if (last_block_piece_ == 0) {
                last_block_piece_ = PIECE_PER_BLOCK;
            } else {
                ++block_count;
            }
            assert(block_count < BLOCK_PER_SEGMENT);
            blocks_.resize(block_count);
        }

        boost::uint16_t Segment::piece_size(
            boost::uint64_t id) const
        {
            boost::uint32_t index = BLOCK_PIECE(id);
            if (index < (boost::uint32_t(piece_count_ - 1))) {
                return PIECE_SIZE;
            } else if (index < piece_count_) {
                return last_piece_size_;
            } else {
                assert(false);
                return 0;
            }
        }

        Block const & Segment::get_block(
            boost::uint64_t id) const
        {
            boost::uint16_t index = BLOCK(id);
            assert(index < blocks_.size());
            if (index < blocks_.size() && blocks_[index]) {
                return *blocks_[index];
            }
            static Block empty_block(0);
            return empty_block;
        }

        Piece::pointer Segment::get_piece(
            boost::uint64_t id) const
        {
            return get_block(id).get_piece(id);
        }

        md5_bytes Segment::cache_md5sum() const
        {
            framework::string::Md5 md5;
            for (size_t i = 0; i < blocks_.size(); ++i) {
                std::vector<Piece::pointer> const & pieces(blocks_[i]->pieces());
                for (size_t j = 0; i < pieces.size(); ++j) {
                    md5.update(pieces[j]->data, pieces[j]->size);
                }
            }
            md5.final();
            return md5.to_bytes();
        }

        md5_bytes Segment::file_md5sum(
            boost::filesystem::path const & path)
        {
            framework::string::Md5 md5;
            framework::filesystem::File file;
            boost::system::error_code ec;
            file.open(path, file.f_read, ec);
            static std::vector<boost::uint8_t> buf(4096, 0);
            size_t bytes_read = 0;
            while ((bytes_read = file.read_some(boost::asio::buffer(buf), ec)))
                md5.update(&buf[0], bytes_read);
            md5.final();
            return md5.to_bytes();
        }

        bool Segment::save(
            boost::filesystem::path const & path) const
        {
            framework::filesystem::File file;
            boost::system::error_code ec;
            if (!file.open(path, file.f_write, ec))
                return false;
            for (size_t i = 0; i < blocks_.size(); ++i) {
                std::vector<Piece::pointer> const & pieces(blocks_[i]->pieces());
                for (size_t j = 0; i < pieces.size(); ++j) {
                    if (!file.write_some(boost::asio::buffer(pieces[j]->data, pieces[j]->size), ec))
                        return false;
                }
            }
            return file.close(ec);
        }

        boost::uint64_t Segment::set_piece(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            boost::uint64_t next_id = modify_block(id).set_piece(id, piece);
            if (next_id == MAKE_ID(0, 0, MAX_PIECE)) {
                boost::uint16_t index = BLOCK(id);
                if (next_ == index) {
                    ++next_;
                    while (next_ < blocks_.size() && (blocks_[next_]->finished()))
                        ++next_;
                    if (next_ == blocks_.size()) {
                        next_ = MAX_BLOCK;
                        return MAKE_ID(0, next_, next_id);
                    }
                }
            }
            return MAKE_ID(0, next_, blocks_[next_]->next());
        }

        Block const * Segment::map_block(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            boost::uint16_t index = BLOCK(id);
            assert(index < blocks_.size() && blocks_[index] == NULL);
            if (index >= blocks_.size() || blocks_[index]) {
                return NULL;
            }
            boost::uint32_t offset = BLOCK_SIZE * index;
            size_t size = BLOCK_SIZE;
            if (index == blocks_.size() - 1) {
                size = (last_block_piece_ - 1) * PIECE_SIZE + last_piece_size_;
            }
            framework::filesystem::File file;
            boost::system::error_code ec;
            if (!file.open(path, file.f_read, ec))
                return NULL;
            void * addr = file.map(offset, size, file.fm_read, ec);
            if (addr == NULL)
                return NULL;
            blocks_[index] = new Block(addr, size);
            return blocks_[index];
        }

        bool Segment::unmap_block(
            boost::uint64_t id)
        {
            boost::uint16_t index = BLOCK(id);
            assert(index < blocks_.size() && blocks_[index]);
            if (index >= blocks_.size() || blocks_[index] == NULL) {
                return false;
            }
            boost::uint32_t offset = BLOCK_SIZE * index;
            size_t size = BLOCK_SIZE;
            if (index == blocks_.size() - 1) {
                size = (last_block_piece_ - 1) * PIECE_SIZE + last_piece_size_;
            }
            framework::filesystem::File file;
            boost::system::error_code ec;
            if (!file.unmap(blocks_[index]->get_piece(0)->data, offset, size, ec))
                return false;
            delete blocks_[index];
            blocks_[index] = NULL;
            return true;
        }

        Block & Segment::modify_block(
            boost::uint64_t id)
        {
            static Block empty_block(0);
            boost::uint16_t index = BLOCK(id);
            if (index < blocks_.size()) {
                if (blocks_[index] == NULL) {
                    if (index < blocks_.size() - 1) {
                        blocks_[index] = new Block(PIECE_PER_BLOCK);
                    } else {
                        blocks_[index] = new Block(last_block_piece_);
                    }
                }
                return *blocks_[index];
            } else {
                assert(false);
                return empty_block;
            }
        }

    } // namespace client
} // namespace trip

