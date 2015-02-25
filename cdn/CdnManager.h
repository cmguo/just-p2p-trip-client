// CdnManager.h

#ifndef _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
#define _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_

#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/WorkQueue.h"

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class ResourceManager;

        class CdnManager
            : public util::daemon::ModuleBase<CdnManager>
        {
        public:
            CdnManager(
                util::daemon::Daemon & daemon);

            ~CdnManager();

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void on_event(
                util::event::Event const & event);

        private:
            ResourceManager & res_mgr_;
            std::vector<Resource *> resources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
