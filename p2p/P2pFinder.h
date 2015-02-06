// P2pFinder.h

#ifndef _TRIP_CLIENT_P2P_P2P_FINDER_H_
#define _TRIP_CLIENT_P2P_P2P_FINDER_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/core/Finder.h"

namespace trip
{
    namespace client
    {

        class P2pFinder
            : Finder
            , UdpSession
        {
        public:
            P2pFinder(
                UdpTunnel & tunnel);

            virtual ~P2pFinder();

        public:
            static P2pFinder * create(
                boost::asio::io_service & io_svc);

        public:
            virtual void find_more(
                Resource & resource);
            
        private:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_FINDER_H_
