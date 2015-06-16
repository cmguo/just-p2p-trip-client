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

        static PoolPiece::oom_handler_t oom_handler = NULL;
        static void * handler_data = NULL;

        void PoolPiece::set_oom_handler(
            oom_handler_t handler, 
            void * data)
        {
            oom_handler = handler;
            handler_data = data;
        }

        PoolPiece * PoolPiece::alloc(
            boost::uint16_t size)
        {
            void * ptr = pool.alloc(sizeof(PoolPiece));
            if (ptr)
                return new (ptr) PoolPiece(size);
            else if (oom_handler) {
                size_t level = 0;
                do {
                    oom_handler(handler_data, level);
                    ptr = pool.alloc(sizeof(PoolPiece));
                    if (ptr)
                        return new (ptr) PoolPiece(size);
                } while (level < 6);
                return NULL;
            } else {
                return NULL;
            }
        }

        void PoolPiece::free(
            PoolPiece * p)
        {
            p->~PoolPiece();
            pool.free(p);
        }

    } // namespace client
} // namespace trip
