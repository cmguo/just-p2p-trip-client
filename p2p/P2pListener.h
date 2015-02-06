// P2pListener.h

#ifndef _TRIP_CLIENT_P2P_P2P_LISTENER_H_
#define _TRIP_CLIENT_P2P_P2P_LISTENER_H_

#include "trip/client/udp/UdpMainSession.h"

namespace trip
{
    namespace client
    {

        class P2pListener
            : public UdpMainSession
        {
        public:
            P2pListener(
                UdpTunnel & tunnel);

            virtual ~P2pListener();

        private:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_LISTENER_H_
