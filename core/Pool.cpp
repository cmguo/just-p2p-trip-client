// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Block.h"
#include "trip/client/core/Segment.h"
#include "trip/client/core/Memory.h"
#include "trip/client/core/ResourceData.h"

#include <framework/memory/BigFixedPool.h>
#include <framework/memory/PrivateMemory.h>

namespace trip
{
    namespace client
    {

        static framework::memory::BigFixedPool pool(framework::memory::PrivateMemory(), -1, sizeof(Segment));

        Segment * Segment::alloc(
            boost::uint32_t size)
        {
            void * ptr = pool.alloc(sizeof(Segment));
            if (ptr)
                return new (ptr) Segment(size);
            ptr = Memory::inst().oom_alloc(pool, sizeof(Segment));;
            if (ptr)
                return new (ptr) Segment(size);
            return NULL;
        }

        void Segment::free(
            Segment *& p)
        {
            p->~Segment();
            pool.free(p);
            p = NULL;
        }

        Block * Block::alloc(
            boost::uint32_t size)
        {
            void * ptr = pool.alloc(sizeof(Block));
            if (ptr)
                return new (ptr) Block(size);
            ptr = Memory::inst().oom_alloc(pool, sizeof(Block));
            if (ptr)
                return new (ptr) Block(size);
            return NULL;
        }

        void Block::free(
            Block *& p)
        {
            p->~Block();
            pool.free(p);
            p = NULL;
        }

        framework::memory::MemoryPool & ResourceData::mpool()
        {
            return pool;
        }

    } // namespace client
} // namespace trip
