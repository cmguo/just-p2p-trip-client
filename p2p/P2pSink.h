// P2pSink.h

#ifndef _TRIP_CLIENT_P2P_P2P_SINK_H_
#define _TRIP_CLIENT_P2P_P2P_SINK_H_

#include "trip/client/udp/UdpSession.h"

namespace trip
{
    namespace client
    {

        class ResourceManager;
        class Resource;

        class P2pSink
            : UdpSession
        {
        public:
            P2pSink(
                Resource & resource, 
                UdpTunnel & tunnel);

            virtual ~P2pSink();

        public:
            static UdpSession * create_session(
                ResourceManager & rmgr, 
                UdpTunnel & tunnel, 
                Message & msg);

        public:
            virtual void on_msg(
                Message * msg);

        private:
            Resource & resource_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_SINK_H_
