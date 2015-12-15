// MemoryManager.h

#ifndef _TRIP_CLIENT_MAIN_MEMORY_MANAGER_H_
#define _TRIP_CLIENT_MAIN_MEMORY_MANAGER_H_

#include <util/daemon/Module.h>
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
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_MEMORY_MANAGER_H_
