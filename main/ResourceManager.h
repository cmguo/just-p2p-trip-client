// ResourceManager.h

#ifndef _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_
#define _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_

#include "trip/client/main/ResourceMapping.h"
#include "trip/client/core/ResourceEvent.h"

#include <util/daemon/Module.h>
#include <util/event/Observable.h>

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        class Resource;

        class ResourceManager
            : public util::daemon::ModuleBase<ResourceManager>
            , public util::event::Observable
        {
        public:
            ResourceManager(
                util::daemon::Daemon & daemon);

            ~ResourceManager();

        public:
            ResourceChangedEvent resource_added;

            ResourceChangedEvent resource_deleting;

        public:
            Resource & get(
                Uuid const & rid);

            Resource & get(
                std::vector<Url> & urls);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

        private:
            boost::filesystem::path path_;
            std::map<Uuid, Resource *> resources_;
            std::vector<Resource *> other_resources_;
            ResourceMapping mapping_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_
