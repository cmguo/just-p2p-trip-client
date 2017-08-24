// DataGraph.cpp

#include "trip/client/Common.h"
#include "trip/client/Version.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/core/Serialize.h"
#include "trip/client/main/Serialize.h"
#include "trip/client/udp/Serialize.h"
#include "trip/client/ssp/Serialize.h"
#include "trip/client/p2p/Serialize.h"
#include "trip/client/cdn/Serialize.h"
#include "trip/client/cache/Serialize.h"
#include "trip/client/sink/Serialize.h"
#include "trip/client/download/Serialize.h"
#include "trip/client/http/Serialize.h"

#include <util/datagraph/WalkArchive.h>
#include <util/archive/JsonOArchive.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.DataGraph", framework::logger::Debug);

        class WalkPrint
            : public util::datagraph::WalkArchive<WalkPrint>
        {
        public:
            WalkPrint(
                std::ostream & os)
                : os_(os)
                , found_(false)
            {
            }

        public:
            friend class util::archive::SaveAccess;

            template <typename T>
            void print(
                T const & t, 
                std::string const & path, 
                int depth = 65536)
            {
                set_path(path);
                depth_ = depth;
                (*this) << t;
                if (!found_) {
                    os_ << "<not found>";
                }
            }

            template <typename T>
            void found(
                T const & t)
            {
                util::archive::JsonOArchive<> json(os_);
                json.set_depth(depth_);
                json << t;
                found_ = true;
            }

        private:
            std::ostream & os_;
            bool found_;
            int depth_;
        };

        struct DataRoot
        {
            DataRoot(
                util::daemon::Daemon & daemon)
                : version(trip::client::version_string())
                , bootstrap(util::daemon::use_module<Bootstrap>(daemon))
                , resource(util::daemon::use_module<ResourceManager>(daemon))
                , memory(util::daemon::use_module<MemoryManager>(daemon))
                , cache(util::daemon::use_module<CacheManager>(daemon))
                , udp(util::daemon::use_module<UdpManager>(daemon))
                , ssp(util::daemon::use_module<SspManager>(daemon))
                , p2p(util::daemon::use_module<P2pManager>(daemon))
                , cdn(util::daemon::use_module<CdnManager>(daemon))
                , sink(util::daemon::use_module<SinkManager>(daemon))
                , download(util::daemon::use_module<DownloadManager>(daemon))
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar 
                    & SERIALIZATION_NVP(bootstrap)
                    & SERIALIZATION_NVP(resource)
                    & SERIALIZATION_NVP(memory)
                    & SERIALIZATION_NVP(cache)
                    & SERIALIZATION_NVP(udp)
                    & SERIALIZATION_NVP(ssp)
                    & SERIALIZATION_NVP(p2p)
                    & SERIALIZATION_NVP(cdn)
                    & SERIALIZATION_NVP(sink)
                    & SERIALIZATION_NVP(download)
                    ;
            }

            std::string version;
            Bootstrap const & bootstrap;
            ResourceManager const & resource;
            MemoryManager const & memory;
            CacheManager const & cache;
            UdpManager const & udp;
            SspManager const & ssp;
            P2pManager const & p2p;
            CdnManager const & cdn;
            SinkManager const & sink;
            DownloadManager const & download;

            static DataRoot & inst(
                util::daemon::Daemon & daemon = *(util::daemon::Daemon *)NULL)
            {
                static DataRoot data_root(daemon);
                return data_root;
            }
        };

        class ConfigModule
            : public framework::configure::ConfigModuleBase
        {
        public:
            ConfigModule(
                DataGraph & graph)
                : graph_(graph)
            {
            }

            boost::system::error_code set(
                std::string const & key, 
                std::string const & value)
            {
                return framework::system::logic_error::no_permission;
            }

            boost::system::error_code get(
                std::string const & key, 
                std::string & value) const
            {
                std::ostringstream os;
                if (graph_.dump(key, os)) {
                    value = os.str();
                    return boost::system::error_code();
                } else {
                    return framework::system::logic_error::item_not_exist;
                }
            }

            boost::system::error_code get(
                std::map<std::string, std::string> & kvs) const
            {
                return boost::system::error_code();
            }

        private:
            DataGraph & graph_;
        };

        DataGraph::DataGraph(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<DataGraph>(daemon, "trip.client.DataGraph")
        {
            DataRoot::inst(daemon);
            config().register_module(name(), new ConfigModule(*this));
        }

        DataGraph::~DataGraph()
        {
        }

        bool DataGraph::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool DataGraph::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool DataGraph::dump(
            std::string const & path, 
            std::ostream & os, 
            int depth) const
        {
            WalkPrint wp(os);
            wp.print(DataRoot::inst(), path, depth);
            return wp;
        }

        bool DataGraph::expandUrl(
            Url & url, 
            boost::system::error_code & ec) const
        {
            for (Url::param_iterator iter = url.param_begin(); 
                    iter != url.param_end(); ++iter) {
                Url::Parameter & p = *iter;
                std::string v = p.value();
                if (v.size() > 2 && v[0] == '<' && v[v.size() - 1] == '>') {
                    v = v.substr(1, v.size() - 2);
                    std::ostringstream oss;
                    if (dump(v, oss)) 
                        p = oss.str();
                }
            }
            return true;
        }
        
    } // namespace client
} // namespace trip
