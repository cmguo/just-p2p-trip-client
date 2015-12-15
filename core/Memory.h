// Memory.h

#ifndef _TRIP_CLIENT_CORE_MEMORY_H_
#define _TRIP_CLIENT_CORE_MEMORY_H_

namespace trip
{
    namespace client
    {

        class Memory
        {
        public:
            static Memory & inst()
            {
                static Memory memory;
                return memory;
            }

            Memory()
                : handler_(&default_oom_handler)
                , ctx_(NULL)
            {
            }

        public:
            typedef void (*oom_handler_t)(
                void const * ctx, 
                size_t level);

            void set_oom_handler(
                oom_handler_t handler, 
                void const * ctx)
            {
                handler_ = handler;
                ctx_ = ctx;
            }

        public:
            template <typename Pool>
            inline void * oom_alloc(
                Pool & pool, 
                size_t size)
            {
                size_t level = 0;
                do {
                    handler_(ctx_, level);
                    void * ptr = pool.alloc(size);
                    if (ptr) return ptr;
                } while (level < 6);
                assert(false);
                return NULL;
            }

        private:
            static void default_oom_handler(
                void const * ctx, 
                size_t level)
            {
                // Do nothing
            }

        private:
            oom_handler_t handler_;
            void const * ctx_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_MEMORY_H_
