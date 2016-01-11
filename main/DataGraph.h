// DataGraph.h

#ifndef _TRIP_CLIENT_MAIN_DATA_GRAPH_H_
#define _TRIP_CLIENT_MAIN_DATA_GRAPH_H_

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class DataGraph
            : public util::daemon::ModuleBase<DataGraph>
        {
        public:
            DataGraph(
                util::daemon::Daemon & daemon);

            ~DataGraph();

        public:
            bool dump(
                std::string const & path, 
                std::ostream & os);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_DATA_GRAPH_H_
