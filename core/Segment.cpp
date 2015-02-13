// Segment.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Segment.h"
#include "trip/client/core/BlockData.h"

#include <framework/filesystem/File.h>

namespace trip
{
    namespace client
    {

        Segment::Segment(
            boost::uint32_t size)
            : next_(0)
        {
            set_size(size);
        }

        boost::uint32_t Segment::size() const
        {
            return BLOCK_SIZE * block_count_ - BLOCK_SIZE + last_block_size_;
        }

        Block const * Segment::get_block(
            boost::uint64_t id) const
        {
            boost::uint16_t index = BLOCK(id);
            assert(index < blocks_.size());
            if (index < blocks_.size()) {
                return blocks_[index];
            }
            return NULL;
        }

        Piece::pointer Segment::get_piece(
            boost::uint64_t id) const
        {
            Block const * block = get_block(id);
            if (block == NULL)
                return NULL;
            return block->get_piece(id);
        }

        void Segment::get_stat(
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
            for (size_t i = 0; i < pieces_.size() - 1; ++i) {
                map.push_back(blocks_[i] && blocks_[i]->finished());
            }
        }

        md5_bytes Segment::cache_md5sum() const
        {
            framework::string::Md5 md5;
            for (size_t i = 0; i < blocks_.size(); ++i) {
                std::vector<Piece::pointer> const & pieces(blocks_[i]->pieces());
                for (size_t j = 0; i < pieces.size(); ++j) {
                    md5.update(pieces[j]->data(), pieces[j]->size());
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
                    if (!file.write_some(boost::asio::buffer(pieces[j]->data(), pieces[j]->size()), ec))
                        return false;
                }
            }
            return file.close(ec);
        }

        void Segment::set_size(
            boost::uint32_t size)
        {
            block_count_ = size / BLOCK_SIZE;
            last_block_size_ = size % BLOCK_SIZE;
            if (last_block_size_ == 0) {
                last_block_size_ = BLOCK_SIZE;
            } else {
                ++block_count_;
            }
            assert(block_count <= BLOCK_PER_SEGMENT);
            blocks_.resize(block_count_);
        }

        bool Segment::load_block_stat(
            boost::uint64_t id, 
            boost::dynamic_bitset<boost::uint32_t> & map)
        {
            modify_block(id).get_stat(map);
            return true;
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
                        return MAKE_ID(0, next_, 0);
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
            BlockData * data = BlockData::alloc(path, offset, size);
            blocks_[index] = new Block(data);
            return blocks_[index];
        }

        Block & Segment::modify_block(
            boost::uint64_t id)
        {
            boost::uint16_t index = BLOCK(id);
            if (index < blocks_.size()) {
                if (blocks_[index] == NULL) {
                    if (index < blocks_.size() - 1) {
                        blocks_[index] = new Block(BLOCK_SIZE);
                    } else {
                        blocks_[index] = new Block(last_block_size_);
                    }
                }
                return *blocks_[index];
            } else {
                assert(false);
                static Block empty_block;
                return empty_block;
            }
        }

        void Segment::release(
            boost::uint64_t from,
            boost::uint64_t to)
        {
            boost::uint64_t blkf = BLOCK(from);
            boost::uint64_t blkt = BLOCK(to);
            while (blkf <= blkt) {
                Block *& blk = blocks_[blkf];
                boost::uint64_t f = from;
                ++blkf;
                from = MAKE_ID(0, blkf, 0);
                if (blk == NULL)
                    continue;
                if (blkf == blkt) {
                    blk->release(f, to);
                } else if (PIECE(from) != 0) {
                    blk->release(f, from);
                } else {
                    delete blk;
                    blk = NULL;
                }
            } 
        }

    } // namespace client
} // namespace trip

