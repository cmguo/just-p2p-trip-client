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
            set_size(size);
        }

        boost::uint32_t Segment::size() const
        {
            return BLOCK_SIZE * block_count_ - BLOCK_SIZE + last_block_size_;
        }

        Block const * Segment::get_block(
            DataId id) const
        {
            boost::uint16_t index = id.block;
            assert(index < blocks_.size());
            if (index < blocks_.size()) {
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

        void Segment::get_stat(
            boost::dynamic_bitset<boost::uint32_t> & map) const
        {
            for (size_t i = 0; i < blocks_.size() - 1; ++i) {
                map.push_back(blocks_[i] && blocks_[i]->full());
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
            assert(block_count_ <= BLOCK_PER_SEGMENT);
            assert(blocks_.size() <= block_count_);
            for (size_t i = block_count_; i < blocks_.size(); ++i) {
                delete blocks_[i];
            }
            blocks_.resize(block_count_);
            left_ = 0;
            for (size_t i = 0; i < blocks_.size(); ++i) {
                if (!blocks_[i]->full())
                    ++left_;
            }
            blocks_.back()->set_size(last_block_size_);
        }

        bool Segment::load_block_stat(
            DataId id, 
            boost::dynamic_bitset<boost::uint32_t> & map)
        {
            modify_block(id).get_stat(map);
            return true;
        }

        bool Segment::seek(
            DataId & id)
        {
            boost::uint16_t next = id.block;
            if (next < blocks_.size()) {
                if (blocks_[next]) {
                    if (!blocks_[next]->seek(id))
                        return false;
                    ++next;
                }
            }
            while (next < blocks_.size() && blocks_[next] && blocks_[next]->full())
                ++next;
            id.piece = 0;
            if (next >= blocks_.size()) {
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
            assert(index < blocks_.size() && blocks_[index] == NULL);
            if (index >= blocks_.size() || blocks_[index]) {
                return NULL;
            }
            boost::uint32_t offset = BLOCK_SIZE * index;
            size_t size = BLOCK_SIZE;
            if (index == blocks_.size() - 1) {
                size = last_block_size_;
            }
            BlockData * data = BlockData::alloc(path, offset, size);
            blocks_[index] = new Block(data);
            --left_;
            return blocks_[index];
        }

        Block & Segment::modify_block(
            DataId id)
        {
            boost::uint16_t index = id.block;
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
            DataId from,
            DataId to)
        {
            boost::uint32_t blkf = from.block;
            boost::uint32_t blkt = to.block_piece ? to.block : blocks_.size();
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
            }
            for (; blkf < blkt; ++blkf, ++blkp) {
                if (*blkp) {
                    delete *blkp; 
                    *blkp = NULL;
                    ++left_;
                }
            } 
            if (to.piece && *blkp) {
                if ((*blkp)->full())
                    ++left_;
                (*blkp)->release(DataId(0, blkf, 0), to);
            }
        }

    } // namespace client
} // namespace trip

