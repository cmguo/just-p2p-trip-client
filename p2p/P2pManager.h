// P2pManager.h

#ifndef _TRIP_P2P_P2P_MANAGER_H_
#define _TRIP_P2P_P2P_MANAGER_H_

#include "trip/client/proto/Message.h"

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        struct UdpEndpoint;

        class ResourceManager;
        class Resource;
        class UdpManager;
        class Source;
        class Sink;
        class Finder;
        class P2pFinder;
        class P2pSource;
        class P2pSink;
        class UdpTunnel;
        class UdpSession;

        //typedef UdpTunnel P2pTunnel;

        class P2pManager
            : public util::daemon::ModuleBase<P2pManager>
        {
        public:
            P2pManager(
                util::daemon::Daemon & daemon);

            ~P2pManager();

        public:
            Finder * finder();

        public:
            typedef std::map<Uuid, // rid
                    std::map<Uuid, P2pSource *> > source_map_t;
            typedef std::map<Uuid, // rid
                    std::map<Uuid, P2pSink *> > sink_map_t;

            source_map_t const & sources() const
            {
                return sources_;
            }

            sink_map_t const & sinks() const
            {
                return sinks_;
            }

        private:
            friend class P2pFinder;

            void get_sources(
                Uuid const & rid, 
                std::vector<UdpEndpoint> const & endpoints, 
                std::vector<Source *> & sources);

            Source * get_source(
                Resource & resource, 
                UdpEndpoint const & endpoint);

        private:
            UdpSession * create_session(
                UdpTunnel & tunnel, 
                Message & msg);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            ResourceManager & rmgr_;
            UdpManager & umgr_;
            P2pFinder * finder_;
            source_map_t sources_;
            sink_map_t sinks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_P2P_MANAGER_H_
