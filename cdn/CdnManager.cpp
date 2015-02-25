// CdnManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Resource.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        CdnManager::CdnManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CdnManager>(daemon, "CdnManager")
            , res_mgr_(util::daemon::use_module<ResourceManager>(daemon))
        {
            res_mgr_.resource_added.on(
                boost::bind(&CdnManager::on_event, this, _2));
            res_mgr_.resource_deleting.on(
                boost::bind(&CdnManager::on_event, this, _2));
        }

        CdnManager::~CdnManager()
        {
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

        void CdnManager::on_event(
            util::event::Event const & event)
        {
            if (event == res_mgr_.resource_added) {
                if (res_mgr_.resource_added.resource->mata() == NULL) {
                    resources_.push_back(res_mgr_.resource_added.resource);
                }
            } else {
                resources_.erase(
                    std::remove(resources_.begin(), resources_.end(), res_mgr_.resource_deleting.resource), 
                    resources_.end());
            }
        }

    } // namespace client
} // namespace trip
