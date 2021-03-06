// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/Memory.h"

#include <framework/memory/BigFixedPool.h>
#include <framework/memory/PrivateMemory.h>

namespace trip
{
    namespace client
    {

        static framework::memory::BigFixedPool pool(framework::memory::PrivateMemory(), -1, sizeof(PoolPiece));

        PoolPiece * PoolPiece::alloc(
            boost::uint16_t size)
        {
            void * ptr = pool.alloc(sizeof(PoolPiece));
            if (ptr)
                return new (ptr) PoolPiece(size);
            ptr = Memory::inst().oom_alloc(pool, sizeof(PoolPiece));;
            if (ptr)
                return new (ptr) PoolPiece(size);
            return NULL;
        }

        void PoolPiece::free(
            PoolPiece * p)
        {
            p->~PoolPiece();
            pool.free(p);
        }

        framework::memory::MemoryPool & PoolPiece::mpool()
        {
            return pool;
        }

    } // namespace client
} // namespace trip
