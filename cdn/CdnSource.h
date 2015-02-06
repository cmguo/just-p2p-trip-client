// CdnSource.h

#ifndef _TRIP_CLIENT_CDN_CDN_SOURCE_H_
#define _TRIP_CLIENT_CDN_CDN_SOURCE_H_

#include "trip/client/core/Source.h"

#include <util/protocol/http/HttpClient.h>

namespace trip
{
    namespace client
    {

        class CdnSource
            : Source
        {
        public:
            CdnSource(
                boost::asio::io_service & io_svc,
                Resource & resource);

            virtual ~CdnSource();

        public:
            static CdnSource * create(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

        public:
            virtual bool request(
                std::vector<boost::uint64_t> & pieces);
            
        private:
            void handle_open(
                boost::system::error_code ec);

            void handle_read(
                boost::system::error_code ec, 
                size_t bytes_read, 
                Piece::pointer piece);

        private:
            util::protocol::HttpClient http_;
            size_t index_;
            std::vector<boost::uint64_t> pieces_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CDN_CDN_SOURCE_H_
