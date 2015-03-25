// P2pFinder.h

#ifndef _TRIP_CLIENT_P2P_P2P_FINDER_H_
#define _TRIP_CLIENT_P2P_P2P_FINDER_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/core/Finder.h"

namespace trip
{
    namespace client
    {

        class UdpManager;

        struct UdpPeer;

        class P2pFinder
            : public Finder
            , UdpSession
        {
        public:
            P2pFinder(
                UdpManager & manager);

            virtual ~P2pFinder();

        public:
            virtual std::string proto() const;

        private:
            virtual void find(
                Resource & resource, 
                size_t count);

            virtual Source * create(
                Resource & resource, 
                Url const & url);
            
        private:
            virtual void on_msg(
                Message * msg);

        private:
            UdpManager & umgr_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_FINDER_H_
