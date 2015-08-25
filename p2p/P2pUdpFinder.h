// P2pUdpFinder.h

#ifndef _TRIP_CLIENT_P2P_P2P_UDP_FINDER_H_
#define _TRIP_CLIENT_P2P_P2P_UDP_FINDER_H_

#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/udp/UdpSession.h"

namespace trip
{
    namespace client
    {

        class P2pUdpFinder
            : public P2pFinder
            , UdpSession
        {
        public:
            P2pUdpFinder(
                P2pManager & manager);

            virtual ~P2pUdpFinder();

        private:
            virtual void send_msg(
                Message const & msg);

        private:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_UDP_FINDER_H_
