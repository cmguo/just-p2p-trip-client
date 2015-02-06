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
            : util::daemon::ModuleBase<ResourceManager>(daemon)
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
                iter = resources_.insert(std::make_pair(rid, new Resource)).first;
                Resource & r = *iter->second;
                r.set_id(rid);
            }
            return *iter->second;
        }

        Resource & ResourceManager::get(
            std::vector<Url> const & urls)
        {
            Uuid rid;
            rid.generate();
            Resource & r = get(rid);
            r.set_urls(urls);
            return r;
        }

    } // namespace client
} // namespace trip
