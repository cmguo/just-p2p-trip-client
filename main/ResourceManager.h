// ResourceManager.h

#ifndef _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_
#define _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_

#include "trip/client/core/ResourceEvent.h"

#include <util/daemon/Module.h>
#include <util/event/Observable.h>
#include <util/protocol/http/HttpClient.h>

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
            ResourceEvent resource_added;

            ResourceEvent resource_deleting;

        public:
            Resource & get(
                Uuid const & rid);

            Resource * find(
                Uuid const & rid);

            void async_get(
                std::vector<Url> & urls, 
                ResourceEvent & event);

            void get_urls(
                Uuid const & rid, 
                std::vector<Url> & urls);

            std::map<Uuid, Resource *> const & resources()
            {
                return resources_;
            }

            boost::system::error_code const & last_error() const
            {
                return last_error_;
            }

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

            virtual void dump() const;

        private:
            void on_event();

            void handle_fetch(
                boost::system::error_code ec, 
                ResourceEvent & event);

        private:
            Url url_;
            util::protocol::HttpClient http_;
            std::map<Uuid, Resource *> resources_;
            std::map<Uuid, std::vector<Url> > resource_urls_;
            boost::system::error_code last_error_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_RESOURCE_MANAGER_H_
