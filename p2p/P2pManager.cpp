// P2pManager.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/p2p/P2pHttpFinder.h"
#include "trip/client/p2p/P2pUdpFinder.h"
#include "trip/client/p2p/P2pSink.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.P2pManager", framework::logger::Debug);

        P2pManager::P2pManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<P2pManager>(daemon, "trip.client.P2pManager")
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
            , umgr_(util::daemon::use_module<UdpManager>(io_svc()))
            , finder_(new P2pUdpFinder(*this))
        {
            umgr_.register_service(REQ_Bind, 
                boost::bind(&P2pManager::get_sink, this, _1, _2));
        }

        P2pManager::~P2pManager()
        {
            delete finder_;
        }

        bool P2pManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool P2pManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        Finder * P2pManager::finder()
        {
            return finder_;
        }

        void P2pManager::get_sources(
            Uuid const & rid, 
            std::vector<UdpEndpoint> const & endpoints, 
            std::vector<Source *> & sources)
        {
            Resource * resource(rmgr_.find(rid));
            if (resource == NULL) {
                LOG_WARN("[get_sources] no resource, rid:" << rid.to_string());
                return;
            }
            for (size_t i = 0; i < endpoints.size(); ++i) {
                Source * source = get_source(*resource, endpoints[i]);
                if (source && !source->attached())
                    sources.push_back(source);
            }
        }

        P2pSource * P2pManager::get_source(
            Resource & resource, 
            UdpEndpoint const & endpoint)
        {
            Uuid const & lid(umgr_.local_endpoint().id);
            if (endpoint.id == lid) {
                LOG_WARN("[get_source] self");
                return NULL;
            }
            if (endpoint.endpoints.empty()) {
                LOG_WARN("[get_source] empty endpoints, id: " << endpoint.id);
                return NULL;
            }
            Uuid const & rid(resource.id());
            source_map_t::iterator iter1 = sources_.find(rid);
            if (iter1 == sources_.end()) {
                iter1 = sources_.insert(std::make_pair(rid, std::map<Uuid, P2pSource *>())).first;
            }
            Uuid const & pid(endpoint.id);
            std::map<Uuid, P2pSource *>::iterator iter2 = iter1->second.find(pid);
            if (iter2 == iter1->second.end()) {
                Url url("p2p:///");
                url.host(endpoint.endpoints[0].ip_str());
                url.svc(framework::string::format(endpoint.endpoints[0].port()));
                url.path("/" + endpoint.id.to_string());
                LOG_DEBUG("[get_source] new source, rid=" << resource.id() << ", url=" << url.to_string());
                UdpTunnel & tunnel = umgr_.get_tunnel(endpoint);
                iter2 = iter1->second.insert(std::make_pair(pid, new P2pSource(*this, resource, tunnel, url))).first;
            }
            return iter2->second;
        }

        void P2pManager::del_source(
            P2pSource * source)
        {
            Uuid const & rid(source->resource().id());
            Uuid const & pid(source->tunnel().pid());
            source_map_t::iterator iter1 = sources_.find(rid);
            if (iter1 == sources_.end()) {
                return;
            }
            std::map<Uuid, P2pSource *>::iterator iter2 = iter1->second.find(pid);
            if (iter2 == iter1->second.end()) {
                return;
            }
            iter1->second.erase(iter2);
            if (iter1->second.empty()) {
                sources_.erase(iter1);
            }
        }

        UdpSession * P2pManager::get_sink(
            UdpTunnel & tunnel, 
            Message & msg)
        {
            Uuid rid = P2pSink::get_bind_rid(msg);
            Resource * resource = rmgr_.find(rid);
            if (resource == NULL) {
                return NULL;
            }
            sink_map_t::iterator iter1 = sinks_.find(rid);
            if (iter1 == sinks_.end()) {
                iter1 = sinks_.insert(std::make_pair(rid, std::map<Uuid, P2pSink *>())).first;
            }
            Uuid const & pid = tunnel.pid();
            std::map<Uuid, P2pSink *>::iterator iter2 = iter1->second.find(pid);
            if (iter2 == iter1->second.end()) {
                iter2 = iter1->second.insert(std::make_pair(pid, new P2pSink(*this, *resource, tunnel))).first;
            }
            return iter2->second;
        }

        void P2pManager::del_sink(
            P2pSink * sink)
        {
            Uuid const & rid(sink->resource().id());
            Uuid const & pid(sink->tunnel().pid());
            sink_map_t::iterator iter1 = sinks_.find(rid);
            if (iter1 == sinks_.end()) {
                return;
            }
            std::map<Uuid, P2pSink *>::iterator iter2 = iter1->second.find(pid);
            if (iter2 == iter1->second.end()) {
                return;
            }
            iter1->second.erase(iter2);
            if (iter1->second.empty()) {
                sinks_.erase(iter1);
            }
        }

    } // namespace client
} // namespace trip
