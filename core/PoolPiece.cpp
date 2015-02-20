// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/PoolPiece.h"

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
            else
                return NULL;
        }

        void PoolPiece::free(
            PoolPiece * p)
        {
            p->~PoolPiece();
            pool.free(p);
        }

    } // namespace client
} // namespace trip
