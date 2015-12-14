// BlockData.cpp

#include "trip/client/Common.h"
#include "trip/client/core/BlockData.h"

#include <framework/filesystem/File.h>
#include <framework/memory/SmallFixedPool.h>
#include <framework/memory/PrivateMemory.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.BlockData", framework::logger::Debug);

        static framework::memory::SmallFixedPool pool(framework::memory::PrivateMemory(), -1, sizeof(BlockPiece));

        BlockData::BlockData(
            void * map_addr, 
            boost::uint32_t offset,
            boost::uint32_t size)
            : nref_(0)
            , offset_(offset)
            , size_(size)
            , map_addr_(map_addr)
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
            if (!file.open(path, file.f_read, ec)) {
                LOG_WARN("[alloc] open failed, path:" << path.string() << ", ec:" << ec.message());
                return NULL;
            }
            void * addr = file.map(offset, size, file.fm_read, ec);
            if (addr == NULL) {
                LOG_WARN("[alloc] map failed, ec:" << ec.message());
                return NULL;
            }
            void * ptr = pool.alloc(sizeof(BlockData));
            if (ptr) {
                LOG_TRACE("[alloc] ptr:" << addr << ", size:" << size);
                return new (ptr) BlockData(addr, offset, size);
            } else {
                file.unmap(addr, offset, size, ec);
                assert(false);
                return NULL;
            }
        }

        void BlockData::free(
            BlockData * p)
        {
            LOG_TRACE("[free] ptr:" << (void *)p->map_addr_ << ", size:" << p->size_);
            framework::filesystem::File file;
            boost::system::error_code ec;
            file.unmap(p->map_addr_, p->offset_, p->size_, ec);
            p->~BlockData();
            pool.free(p);
        }

        BlockPiece * BlockPiece::alloc(
            boost::intrusive_ptr<BlockData> block, 
            boost::uint8_t * data, 
            boost::uint16_t size)
        {
            void * ptr = pool.alloc(sizeof(BlockPiece));
            if (ptr) {
                return new (ptr) BlockPiece(block, data, size);
            } else {
                assert(false);
                return NULL;
            }
        }

        void BlockPiece::free(
            BlockPiece * p)
        {
            p->~BlockPiece();
            pool.free(p);
        }

        framework::memory::MemoryPool & BlockData::mpool()
        {
            return pool;
        }

    } // namespace client
} // namespace trip
