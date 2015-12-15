// MemoryManager.cpp

#include "trip/client/Common.h"
#include "trip/client/main/MemoryManager.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/BlockData.h"
#include "trip/client/core/Block.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Memory.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.MemoryManager", framework::logger::Debug);

        MemoryManager::MemoryManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<MemoryManager>(daemon, "MemoryManager")
        {
            LOG_INFO("[sizeof]"                                 // 64bit    32bit   pool
                << " Segment:" << sizeof(Segment)               // 1032     520     1
                << " Block:" << sizeof(Block)                   // 1032     520     1
                << " BlockData:" << sizeof(BlockData)           // 16       12      2
                << " BlockPiece:" << sizeof(BlockPiece)         // 24       12      2
                << " Resource:" << sizeof(Resource)             // 536      308     x
                << " ResourceMeta:" << sizeof(ResourceMeta)     // 40       36      x
                << " Segment2:" << sizeof(Segment2)             // 48       32      x
                << " SegmentMeta:" << sizeof(SegmentMeta)       // 24       24      x
                //<< "Resource::Lock:" << sizeof(Resource::Lock)  // 16       16      x
                );
        }

        MemoryManager::~MemoryManager()
        {
        }

        bool MemoryManager::startup(
            boost::system::error_code & ec)
        {
            Memory::inst().set_oom_handler(oom_handler, this);
            return true;
        }

        bool MemoryManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        static struct {
            char const * name;
            framework::memory::MemoryPool const * pool;
        } const mpools[] = {
            {"PoolPiece", &PoolPiece::mpool()}, 
            {"ResourceData", &ResourceData::mpool()}, 
            {"BlockData", &BlockData::mpool()}
        };

        void MemoryManager::dump() const
        {
            for (size_t i = 0; i < sizeof(mpools) / sizeof(mpools[0]); ++i) {
                framework::memory::MemoryPool const & pool(*mpools[i].pool);
                LOG_INFO("[dump] [mpool] " << mpools[i].name << ": "
                    << pool.capacity() << "/" << pool.consumption() << "/" << pool.peek()
                    << " " << pool.num_block() << "/" << pool.num_object());
            }
        }

        void MemoryManager::oom_handler(
            void const * ctx, 
            size_t level)
        {
            MemoryManager * mgr = (MemoryManager *)ctx;
            mgr->out_of_memory.level = level;
            mgr->raise(mgr->out_of_memory);
        }

        void MemoryManager::check_memory(
            void const * obj)
        {
            for (size_t i = 0; i < sizeof(mpools) / sizeof(mpools[0]); ++i) {
                framework::memory::MemoryPool const & pool(*mpools[i].pool);
                int n = pool.check_object(obj);
                if (n) {
                    LOG_INFO("[check_memory] pool:" << mpools[i].name << ", status:" << n);
                }
            }
        }

    } // namespace client
} // namespace trip