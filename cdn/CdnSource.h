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
            : public Source
        {
        public:
            CdnSource(
                boost::asio::io_service & io_svc,
                Scheduler & scheduler, 
                Url const & url);

            virtual ~CdnSource();

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
            Url const url_;
            util::protocol::HttpClient http_;
            size_t index_;
            std::vector<boost::uint64_t> pieces_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CDN_CDN_SOURCE_H_
