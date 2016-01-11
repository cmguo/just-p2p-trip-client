// P2pHttpFinder.h

#ifndef _TRIP_CLIENT_P2P_P2P_HTTP_FINDER_H_
#define _TRIP_CLIENT_P2P_P2P_HTTP_FINDER_H_

#include "trip/client/p2p/P2pFinder.h"

#include <util/protocol/http/HttpClient.h>

namespace trip
{
    namespace client
    {

        class P2pHttpFinder
            : public P2pFinder
        {
        public:
            P2pHttpFinder(
                P2pManager & manager);

            virtual ~P2pHttpFinder();

        private:
            virtual void init();

            virtual void send_msg(
                Message const & msg);

            virtual boost::uint16_t get_id();

            virtual void set_id(
                boost::uint16_t id);

        private:
            void handle_fetch(
                boost::system::error_code ec);

        private:
            std::vector<Url> urls_;
            util::protocol::HttpClient http_;
            util::protocol::HttpRequest req_;
            std::deque<boost::uint16_t> requests_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_FINDER_H_
