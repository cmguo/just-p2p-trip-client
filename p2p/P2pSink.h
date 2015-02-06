// P2pSink.h

#ifndef _TRIP_CLIENT_P2P_P2P_SINK_H_
#define _TRIP_CLIENT_P2P_P2P_SINK_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/core/Sink.h"

namespace trip
{
    namespace client
    {

        class P2pSink
            : Sink
            , UdpSession
        {
        public:
            P2pSink(
                Resource & resource,
                UdpTunnel & tunnel);

            virtual ~P2pSink();

        public:
            virtual bool response(
                boost::uint64_t index, 
                Piece::pointer piece);
            
        private:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_SINK_H_
