// ResourceManager.h

#ifndef _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_
#define _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_

#include <util/daemon/Module.h>

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        class Resource;

        class ResourceManager
            : public util::daemon::ModuleBase<ResourceManager>
        {
        public:
            ResourceManager(
                util::daemon::Daemon & daemon);

            ~ResourceManager();

        public:
            Resource & get(
                Uuid const & rid);

            Resource & get(
                std::vector<Url> const & urls);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            boost::filesystem::path path_;
            std::map<Uuid, Resource *> resources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_
