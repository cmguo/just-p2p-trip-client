// BlockData.cpp

#include "trip/client/Common.h"
#include "trip/client/core/BlockData.h"

#include <framework/filesystem/File.h>
#include <framework/memory/SmallFixedPool.h>
#include <framework/memory/PrivateMemory.h>

namespace trip
{
    namespace client
    {

        BlockData::BlockData(
            void * map_addr, 
            boost::uint32_t offset,
            boost::uint32_t size)
            : nref_(1)
            , map_addr_(map_addr)
            , offset_(offset)
            , size_(size)
        {
        }

        BlockData::~BlockData()
        {
        }

        BlockData * BlockData::alloc(
            boost::filesystem::path const & path, 
            boost::uint32_t offset, 
            boost::uint32_t size)
        {
            framework::filesystem::File file;
            boost::system::error_code ec;
            if (!file.open(path, file.f_read, ec))
                return NULL;
            void * addr = file.map(offset, size, file.fm_read, ec);
            if (addr == NULL)
                return NULL;
            return new BlockData(addr, offset, size);
        }

        void BlockData::free(
            BlockData * p)
        {
            framework::filesystem::File file;
            boost::system::error_code ec;
            file.unmap(p->map_addr_, p->offset_, p->size_, ec);
            delete p;
        }

        static framework::memory::SmallFixedPool pool(framework::memory::PrivateMemory(), -1, sizeof(BlockPiece));

        BlockPiece * BlockPiece::alloc(
            boost::intrusive_ptr<BlockData> block, 
            boost::uint8_t * data, 
            boost::uint16_t size)
        {
            void * ptr = pool.alloc(sizeof(BlockData));
            if (ptr)
                return new (ptr) BlockPiece(block, data, size);
            else
                return NULL;
        }

        void BlockPiece::free(
            BlockPiece * p)
        {
            p->~BlockPiece();
            pool.free(p);
        }

    } // namespace client
} // namespace trip
