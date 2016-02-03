// CdnManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/CdnFinder.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/ssp/SspManager.h"
#include "trip/client/ssp/SspTunnel.h"
#include "trip/client/ssp/SspEndpoint.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CdnManager", framework::logger::Debug);

        CdnManager::CdnManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CdnManager>(daemon, "trip.client.CdnManager")
            , smgr_(util::daemon::use_module<SspManager>(io_svc()))
            , finder_(new CdnFinder(*this))
        {
        }

        CdnManager::~CdnManager()
        {
            delete finder_;
        }

        bool CdnManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool CdnManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        Finder * CdnManager::finder()
        {
            return finder_;
        }

        CdnSource * CdnManager::get_source(
            Resource & resource, 
            Url const & url)
        {
            Uuid const & rid(resource.id());
            source_map_t::iterator iter1 = sources_.find(rid);
            if (iter1 == sources_.end()) {
                iter1 = sources_.insert(std::make_pair(rid, std::map<Uuid, CdnSource *>())).first;
            }
            Uuid pid; pid.from_bytes(framework::string::md5(url.host_svc()).to_bytes());
            std::map<Uuid, CdnSource *>::iterator iter2 = iter1->second.find(pid);
            if (iter2 == iter1->second.end()) {
                LOG_DEBUG("[get_source] new source, rid=" << resource.id() << ", url=" << url.to_string());
                SspEndpoint ep;
                ep.id = pid;
                ep.endpoint.from_string(url.host_svc());
                SspTunnel & tunnel(smgr_.get_tunnel(ep));
                iter2 = iter1->second.insert(std::make_pair(pid, new CdnSource(*this, tunnel, resource, url))).first;
            }
            return iter2->second;
        }

        void CdnManager::del_source(
            CdnSource * source)
        {
            Uuid const & rid(source->resource().id());
            Uuid const & pid(source->tunnel().pid());
            source_map_t::iterator iter1 = sources_.find(rid);
            if (iter1 == sources_.end()) {
                return;
            }
            std::map<Uuid, CdnSource *>::iterator iter2 = iter1->second.find(pid);
            if (iter2 == iter1->second.end()) {
                return;
            }
            iter1->second.erase(iter2);
            if (iter1->second.empty()) {
                sources_.erase(iter1);
            }
        }

    } // namespace client
} // namespace trip
