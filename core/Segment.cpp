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
            : left_(0)
            , block_count_(0)
        {
            memset(blocks_, 0, sizeof(blocks_));
            set_size(size);
        }

        Segment::~Segment()
        {
            for (boost::uint16_t i = 0; i < block_count_ - 1; ++i) {
                if (blocks_[i])
                    Block::free(blocks_[i]);
            }
        }

        boost::uint32_t Segment::size() const
        {
            return BLOCK_SIZE * block_count_ - BLOCK_SIZE + last_block_size_;
        }

        boost::uint32_t Segment::piece_count() const
        {
            return PIECE_PER_BLOCK * block_count_ - PIECE_PER_BLOCK + Block(last_block_size_).pieces().size();
        }

        Block const * Segment::get_block(
            DataId id) const
        {
            boost::uint16_t index = id.block;
            assert(index < block_count_);
            if (index < block_count_) {
                return blocks_[index];
            }
            return NULL;
        }

        Piece::pointer Segment::get_piece(
            DataId id) const
        {
            Block const * block = get_block(id);
            if (block == NULL)
                return NULL;
            return block->get_piece(id);
        }

        void Segment::get_status(
            DataId from, 
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
            for (boost::uint16_t i = 0; i < block_count_ - 1; ++i) {
                map.push_back(blocks_[i] && blocks_[i]->full());
            }
        }

        Md5Sum Segment::cache_md5sum() const
        {
            framework::string::Md5 md5;
            for (boost::uint16_t i = 0; i < block_count_; ++i) {
                Block::piece_array_t pieces(blocks_[i]->pieces());
                for (boost::uint16_t j = 0; j < (boost::uint16_t)pieces.size(); ++j) {
                    md5.update(pieces[j]->data(), pieces[j]->size());
                }
            }
            md5.final();
            return md5;
        }

        Md5Sum Segment::file_md5sum(
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
            return md5;
        }

        bool Segment::save(
            boost::filesystem::path const & path, 
            boost::system::error_code & ec) const
        {
            framework::filesystem::File file;
            if (!file.open(path, file.f_write | file.f_create, ec))
                return false;
            for (boost::uint16_t i = 0; i < block_count_; ++i) {
                Block::piece_array_t pieces(blocks_[i]->pieces());
                for (boost::uint16_t j = 0; j < (boost::uint16_t)pieces.size(); ++j) {
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
            assert(block_count_ <= BLOCK_PER_SEGMENT);
            assert(block_count_ <= block_count_);
            for (boost::uint16_t i = block_count_; i < block_count_; ++i) {
                assert(false);
                Block::free(blocks_[i]);
            }
            left_ = 0;
            for (boost::uint16_t i = 0; i < block_count_; ++i) {
                if (blocks_[i] == NULL || !blocks_[i]->full())
                    ++left_;
            }
            if (blocks_[block_count_ - 1])
                blocks_[block_count_ - 1]->set_size(last_block_size_);
        }

        bool Segment::seek(
            DataId & id)
        {
            boost::uint16_t next = id.block;
            if (next < block_count_) {
                if (blocks_[next]) {
                    if (!blocks_[next]->seek(id))
                        return false;
                    ++next;
                } else {
                    return false;
                }
            }
            while (next < block_count_ && blocks_[next] && blocks_[next]->full())
                ++next;
            id.piece = 0;
            if (next >= block_count_) {
                id.block = 0;
                return true;
            } else {
                id.block = next;
                if (blocks_[next])
                    blocks_[next]->seek(id);
                return false;
            }
        }

        bool Segment::set_piece(
            DataId id, 
            Piece::pointer piece)
        {
            Block & block = modify_block(id);
            bool result = block.set_piece(id, piece);
            if (result && block.full())
                --left_;
            return result;
        }

        Block const * Segment::map_block(
            DataId id, 
            boost::filesystem::path const & path)
        {
            boost::uint16_t index = id.block;
            boost::uint32_t offset = BLOCK_SIZE * index;
            boost::uint32_t size = BLOCK_SIZE;
            if (index == block_count_ - 1) {
                size = last_block_size_;
            }
            boost::intrusive_ptr<BlockData> data = BlockData::alloc(path, offset, size);
            Block & block(modify_block(id));
            block.set_data(data);
            --left_;
            return &block;
        }

        Block & Segment::modify_block(
            DataId id)
        {
            boost::uint16_t index = id.block;
            if (index < block_count_) {
                if (blocks_[index] == NULL) {
                    if (index < block_count_ - 1) {
                        blocks_[index] = Block::alloc(BLOCK_SIZE);
                    } else {
                        blocks_[index] = Block::alloc(last_block_size_);
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
            DataId from,
            DataId to)
        {
            boost::uint32_t blkf = from.block;
            boost::uint32_t blkt = to.block_piece ? to.block : block_count_;
            Block ** blkp = &blocks_[blkf];
            if (blkf < blkt && from.piece) {
                ++blkf;
                if (*blkp) {
                    if ((*blkp)->full())
                        ++left_;
                    (*blkp)->release(from, DataId(0, blkf, 0));
                    if ((*blkp)->full())
                        --left_;
                }
                ++blkp;
                from.inc_block();
            }
            for (; blkf < blkt; ++blkf, ++blkp) {
                if (*blkp) {
                    if ((*blkp)->full())
                        ++left_;
                    Block::free(*blkp); 
                }
            } 
            if (to.piece && *blkp) {
                if ((*blkp)->full())
                    ++left_;
                (*blkp)->release(from, to);
            }
        }

    } // namespace client
} // namespace trip

