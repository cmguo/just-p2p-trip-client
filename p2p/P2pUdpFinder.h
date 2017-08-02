// P2pUdpFinder.h

#ifndef _TRIP_CLIENT_P2P_P2P_UDP_FINDER_H_
#define _TRIP_CLIENT_P2P_P2P_UDP_FINDER_H_

#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpEndpoint.h"

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
            virtual void init();

            virtual void send_msg(
                Message const & msg);

            virtual boost::uint16_t get_id();

            virtual void set_id(
                boost::uint16_t id);

        private:
            virtual void on_msg(
                Message * msg);

            virtual void on_timer(
                Time const & now);

            virtual void on_tunnel_connecting();

            virtual void on_tunnel_disconnect();

        private:
            template <typename Archive>
            friend void serialize(
                Archive & ar, 
                P2pUdpFinder & t);

            size_t index_start_;
            size_t index_;
            Duration next_try_intv_;
            Time next_try_;
            std::vector<UdpEndpoint> endpoints_;
            std::vector<UdpTunnel *> tunnels_;
            std::vector<Message *> pending_msgs_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_UDP_FINDER_H_
