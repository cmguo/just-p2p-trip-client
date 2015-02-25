// ResourceManager.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/utils/Format.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        ResourceManager::ResourceManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<ResourceManager>(daemon, "ResourceManager")
        {
            config().register_module("ResourceManager")
                << CONFIG_PARAM_NAME_NOACC("path", path_);
        }

        ResourceManager::~ResourceManager()
        {
        }

        bool ResourceManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool ResourceManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        Resource & ResourceManager::get(
            Uuid const & rid)
        {
            std::map<Uuid, Resource *>::iterator iter = 
                resources_.find(rid);
            if (iter == resources_.end()) {
                Resource * r = new Resource;
                iter = resources_.insert(std::make_pair(rid, r)).first;
                resource_added.resource = r;
                raise(resource_added);
                r->set_id(rid);
            }
            return *iter->second;
        }

        Resource & ResourceManager::get(
            std::vector<Url> const & urls)
        {
            Resource * r = new Resource;
            r->set_urls(urls);
            resource_added.resource = r;
            other_resources_.push_back(r);
            raise(resource_added);
            return *r;
        }

    } // namespace client
} // namespace trip
