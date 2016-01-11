// UdpSessionListener2.h

#ifndef _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER2_H_
#define _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER2_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpEndpoint.h"

namespace trip
{
    namespace client
    {

        class UdpManager;

        class UdpSessionListener2
            : public UdpSession
        {
        public:
            UdpSessionListener2(
                UdpManager & manager, 
                UdpTunnel & tunnel, 
                UdpEndpoint const & endpoint, 
                UdpSession * session);

            virtual ~UdpSessionListener2();

        public:
            virtual void on_msg(
                Message * msg);

        protected:
            UdpManager & umgr_;
            UdpEndpointPairs endpoints_;
            UdpSession * session_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER2_H_
