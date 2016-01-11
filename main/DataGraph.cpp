
#include "trip/client/Common.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/main/DataGraph.hpp"

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
                : json_(os)
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
            }

            template <typename T>
            void found(
                T const & t)
            {
                json_ << t;
            }

        private:
            util::archive::JsonOArchive<> json_;
        };

        struct DataRoot
        {
            DataRoot(
                util::daemon::Daemon & daemon)
                : resource(util::daemon::use_module<ResourceManager>(daemon))
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(resource);
            }

            ResourceManager const & resource;

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
