// CdnFinder.h

#ifndef _TRIP_CLIENT_CDN_CDN_FINDER_H_
#define _TRIP_CLIENT_CDN_CDN_FINDER_H_

#include "trip/client/core/Finder.h"

#include <util/protocol/http/HttpClient.h>

namespace trip
{
    namespace client
    {

        class CdnFinder
            : Finder
        {
        public:
            CdnFinder(
                 boost::asio::io_service & io_svc);

            virtual ~CdnFinder();

        public:
            static CdnFinder * create(
                boost::asio::io_service & io_svc);

        public:
            virtual void find_more(
                Resource & resource);
            
        private:
            void handle_fetch(
                boost::system::error_code ec);

        private:
            util::protocol::HttpClient http_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CDN_CDN_FINDER_H_
