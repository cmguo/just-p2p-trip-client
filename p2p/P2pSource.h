// P2pSource.h

#ifndef _TRIP_CLIENT_P2P_P2P_SOURCE_H_
#define _TRIP_CLIENT_P2P_P2P_SOURCE_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/core/Source.h"

namespace trip
{
    namespace client
    {

        class P2pSource
            : Source
            , UdpSession
        {
        public:
            P2pSource(
                Scheduler & scheduler,
                UdpTunnel & tunnel);

            virtual ~P2pSource();

        public:
            static P2pSource * create(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

        public:
            virtual bool request(
                std::vector<boost::uint64_t> & pieces);
            
        private:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_SOURCE_H_
