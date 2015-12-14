// ResourceManager.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/utils/Format.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/PoolPiece.h"
#include "trip/client/core/BlockData.h"
#include "trip/client/core/Block.h"

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
            LOG_INFO("[sizeof]"                                 // 64bit    32bit   pool
                << " Segment:" << sizeof(Segment)               // 1032     520     1
                << " Block:" << sizeof(Block)                   // 1032     520     1
                << " BlockData:" << sizeof(BlockData)           // 16       12      2
                << " BlockPiece:" << sizeof(BlockPiece)         // 24       12      2
                << " Resource:" << sizeof(Resource)             // 536      308     x
                << " ResourceMeta:" << sizeof(ResourceMeta)     // 40       36      x
                << " Segment2:" << sizeof(Segment2)             // 48       32      x
                << " SegmentMeta:" << sizeof(SegmentMeta)       // 24       24      x
                //<< "Resource::Lock:" << sizeof(Resource::Lock)  // 16       16      x
                );
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

        static struct {
            char const * name;
            framework::memory::MemoryPool const * pool;
        } const mpools[] = {
            {"PoolPiece", &PoolPiece::mpool()}, 
            {"ResourceData", &ResourceData::mpool()}, 
            {"BlockData", &BlockData::mpool()}
        };

        void ResourceManager::dump() const
        {
            for (size_t i = 0; i < sizeof(mpools) / sizeof(mpools[0]); ++i) {
                framework::memory::MemoryPool const & pool(*mpools[i].pool);
                LOG_INFO("[dump] [mpool] " << mpools[i].name << ": "
                    << pool.capacity() << "/" << pool.consumption() << "/" << pool.peek()
                    << " " << pool.num_block() << "/" << pool.num_object());
            }
            // resources
            std::map<Uuid, Resource *>::const_iterator iter = 
                resources_.begin();
            for (; iter != resources_.end(); ++iter) {
                Resource const & res(*iter->second);
                LOG_INFO("[dump] [resouce] " << res.id());
            }
        }

        void ResourceManager::check_memory(
            void const * obj)
        {
            for (size_t i = 0; i < sizeof(mpools) / sizeof(mpools[0]); ++i) {
                framework::memory::MemoryPool const & pool(*mpools[i].pool);
                int n = pool.check_object(obj);
                if (n) {
                    LOG_INFO("[check_memory] pool:" << mpools[i].name << ", status:" << n);
                }
            }
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
