// Memory.h

#ifndef _TRIP_CLIENT_CORE_MEMORY_H_
#define _TRIP_CLIENT_CORE_MEMORY_H_

#include <util/event/Event.h>
#include <util/event/Observable.h>

namespace trip
{
    namespace client
    {

        struct OutOfMemoryEvent
            : util::event::Event
        {
            size_t level;
        };

        class Memory
            : public util::event::Observable
        {
        public:
            OutOfMemoryEvent out_of_memory;

            static Memory & inst()
            {
                static Memory memory;
                return memory;
            }

            template <typename Pool>
                inline void * oom_alloc(
                    Pool & pool, 
                    size_t size)
                {
                    size_t level = 0;
                    do {
                        out_of_memory.level = level;
                        raise(out_of_memory);
                        void * ptr = pool.alloc(size);
                        if (ptr) return ptr;
                    } while (level < 6);
                    assert(false);
                    return NULL;
                }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_MEMORY_H_
