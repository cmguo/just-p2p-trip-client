// BlockData.h

#ifndef _TRIP_CLIENT_CORE_BLOCK_DATA_H_
#define _TRIP_CLIENT_CORE_BLOCK_DATA_H_

#include "trip/client/core/Piece.h"

#include <framework/memory/MemoryPool.h>

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        class Block;

        class BlockData
        {
        private:
            BlockData(
                void * map_addr, 
                boost::uint32_t offset,
                boost::uint32_t size);

            ~BlockData();

        public:
            static BlockData * alloc(
                boost::filesystem::path const & path, 
                boost::uint32_t offset, 
                boost::uint32_t size);

            static void free(
                BlockData * p);

            static framework::memory::MemoryPool & mpool();

            friend void intrusive_ptr_add_ref(
                BlockData * p)
            {
                ++p->nref_;
            }

            friend void intrusive_ptr_release(
                BlockData * p)
            {
                if (--p->nref_ == 0) {
                    BlockData::free(p);
                }
            }

        private:
            friend class Block;

            boost::uint16_t nref_;
            boost::uint16_t offset_;
            boost::uint32_t size_;
            void * map_addr_;
        };

        class BlockPiece
            : public Piece
        {
        public:
            static void set_capacity(
                size_t capacity);

        public:
            static BlockPiece * alloc(
                boost::intrusive_ptr<BlockData> block, 
                boost::uint8_t * data, 
                boost::uint16_t size);

            static void free(
                BlockPiece * p);

        private:
            BlockPiece(
                boost::intrusive_ptr<BlockData> block, 
                boost::uint8_t * data, 
                boost::uint16_t size);

            ~BlockPiece() {}

        private:
            boost::intrusive_ptr<BlockData> block_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_BLOCK_DATA_H_
