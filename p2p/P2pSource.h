// P2pSource.h

#ifndef _TRIP_CLIENT_P2P_P2P_SOURCE_H_
#define _TRIP_CLIENT_P2P_P2P_SOURCE_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/core/Source.h"

namespace trip
{
    namespace client
    {

        class Resource;
        class UdpTunnel;

        class P2pSource
            : public Source
            , UdpSession
        {
        public:
            P2pSource(
                Resource & resource,
                UdpTunnel & tunnel, 
                Url const & url);

            virtual ~P2pSource();

        public:
            virtual bool open();
            
            virtual bool close();
            
            virtual bool do_request();
            
        private:
            static UdpTunnel & get_tunnel(
                boost::asio::io_service & io_svc,
                Url const & url);

        private:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_SOURCE_H_
