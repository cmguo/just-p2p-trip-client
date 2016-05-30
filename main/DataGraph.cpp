// DataGraph.cpp

#include "trip/client/Common.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/main/Serialize.h"
#include "trip/client/udp/Serialize.h"
#include "trip/client/ssp/Serialize.h"
#include "trip/client/p2p/Serialize.h"
#include "trip/client/cdn/Serialize.h"
#include "trip/client/cache/Serialize.h"
#include "trip/client/sink/Serialize.h"

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
                std::string const & path)
            {
                set_path(path);
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
                json << t;
                found_ = true;
            }

        private:
            std::ostream & os_;
            bool found_;
        };

        struct DataRoot
        {
            DataRoot(
                util::daemon::Daemon & daemon)
                : bootstrap(util::daemon::use_module<Bootstrap>(daemon))
                , resource(util::daemon::use_module<ResourceManager>(daemon))
                , cache(util::daemon::use_module<CacheManager>(daemon))
                , udp(util::daemon::use_module<UdpManager>(daemon))
                , ssp(util::daemon::use_module<SspManager>(daemon))
                , p2p(util::daemon::use_module<P2pManager>(daemon))
                , cdn(util::daemon::use_module<CdnManager>(daemon))
                , sink(util::daemon::use_module<SinkManager>(daemon))
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar 
                    & SERIALIZATION_NVP(bootstrap)
                    & SERIALIZATION_NVP(resource)
                    & SERIALIZATION_NVP(cache)
                    & SERIALIZATION_NVP(udp)
                    & SERIALIZATION_NVP(ssp)
                    & SERIALIZATION_NVP(p2p)
                    & SERIALIZATION_NVP(cdn)
                    & SERIALIZATION_NVP(sink)
                    ;
            }

            Bootstrap const & bootstrap;
            ResourceManager const & resource;
            CacheManager const & cache;
            UdpManager const & udp;
            SspManager const & ssp;
            P2pManager const & p2p;
            CdnManager const & cdn;
            SinkManager const & sink;

            static DataRoot & inst(
                util::daemon::Daemon & daemon = *(util::daemon::Daemon *)NULL)
            {
                static DataRoot data_root(daemon);
                return data_root;
            }
        };

        DataGraph::DataGraph(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<DataGraph>(daemon, "DataGraph")
        {
            DataRoot::inst(daemon);
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
                std::ostream & os)
        {
            WalkPrint wp(os);
            wp.print(DataRoot::inst(), path);
            return wp;
        }

    } // namespace client
} // namespace trip
