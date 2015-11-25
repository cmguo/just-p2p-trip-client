// ResourceManager.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/utils/Format.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/PoolPiece.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.ResourceManager", framework::logger::Debug);

        ResourceManager::ResourceManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<ResourceManager>(daemon, "ResourceManager")
            , mapping_(io_svc())
        {
            LOG_INFO("[sizeof] Block:" << sizeof(Block) << ", Segment:" << sizeof(Segment));
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
            mapping_.close();
            return true;
        }

        Resource * ResourceManager::find(
            Uuid const & rid)
        {
            std::map<Uuid, Resource *>::iterator iter = 
                resources_.find(rid);
            if (iter == resources_.end()) {
                return NULL;
            } else {
                return iter->second;
            }
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
                r->set_id(rid);
                raise(resource_added);
            }
            return *iter->second;
        }

        Resource & ResourceManager::get(
            std::vector<Url> & urls)
        {
            LOG_INFO("[get] new resource with no rid");
            Resource * r = new Resource;
            resource_added.resource = r;
            other_resources_.push_back(r);
            //raise(resource_added);
            r->meta_changed.on(
                boost::bind(&ResourceManager::on_event, this, _1, _2));
            mapping_.find(*r, urls);
            return *r;
        }

        static void delete_r(
            Resource * r)
        {
            delete r;
        }

        void ResourceManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            LOG_INFO("[on_event] meta_changed");
            Resource & r = (Resource &)observable;
            assert(r.meta_changed == event);
            r.meta_changed.un(
                boost::bind(&ResourceManager::on_event, this, _1, _2));
            std::map<Uuid,  Resource*>::iterator iter = 
                resources_.find(r.id());
            if (iter == resources_.end()) {
                resources_[r.id()] = &r;
                resource_added.resource = &r;
                raise(resource_added);
            } else {
                r.merge(*iter->second);
                io_svc().post(
                    boost::bind(delete_r, &r));
            }
            other_resources_.erase(
                std::remove(other_resources_.begin(), other_resources_.end(), &r), 
                other_resources_.end());
        }

    } // namespace client
} // namespace trip
