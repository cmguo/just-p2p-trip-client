// MemoryManager.h

#ifndef _TRIP_CLIENT_MAIN_MEMORY_MANAGER_H_
#define _TRIP_CLIENT_MAIN_MEMORY_MANAGER_H_

#include <util/daemon/Module.h>
#include <util/event/EventEx.h>
#include <util/event/Observable.h>

#include <framework/container/Array.h>
#include <framework/memory/MemoryPool.h>

namespace trip
{
    namespace client
    {

        struct OutOfMemoryEvent
            : util::event::EventEx<OutOfMemoryEvent>
        {
            size_t level;
            OutOfMemoryEvent(char const * name) 
                : util::event::EventEx<OutOfMemoryEvent>(name) {}
        };

        class MemoryManager
            : public util::daemon::ModuleBase<MemoryManager>
            , public util::event::Observable
        {
        public:
            MemoryManager(
                util::daemon::Daemon & daemon);

            ~MemoryManager();

        public:
            OutOfMemoryEvent out_of_memory;

        public:
            static void check_memory(
                void const * obj);

        public:
            struct Pool
            {
                char const * name;
                framework::memory::MemoryPool const * pool;
            };

            framework::container::Array<Pool const> pools();

        private:
            static void oom_handler(
                void const * ctx, 
                size_t level);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

            virtual void dump() const;

        private:
            static Pool const pools_[];
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_MEMORY_MANAGER_H_
