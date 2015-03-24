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
            : public Finder
            , UdpSession
        {
        public:
            P2pFinder(
                UdpTunnel & tunnel);

            virtual ~P2pFinder();

        public:
            virtual std::string proto() const;

            virtual void find(
                Resource & resource, 
                size_t count, 
                resp_t const & resp);
            
            virtual void cancel(
                Resource & resource);

            virtual Source * create(
                Scheduler & scheduler, 
                Url const & url);

        private:
            virtual void on_msg(
                Message * msg);

        private:
            std::map<Uuid, std::pair<size_t, resp_t> > requests_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_FINDER_H_
