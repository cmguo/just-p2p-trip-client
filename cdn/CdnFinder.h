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
            : public Finder
        {
        public:
            CdnFinder(
                 boost::asio::io_service & io_svc);

            virtual ~CdnFinder();

        private:
            virtual std::string proto() const;

            virtual void find(
                Resource & resource, 
                size_t count);
            
            virtual Source * create(
                Resource & resource, 
                Url const & url);

        private:
            void handle_fetch(
                boost::system::error_code ec, 
                Resource & resource);

        private:
            util::protocol::HttpClient http_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CDN_CDN_FINDER_H_
