// ResourceMapping.h

#ifndef _TRIP_CLIENT_MAIN_RESOURCE_MAPPING_H_
#define _TRIP_CLIENT_MAIN_RESOURCE_MAPPING_H_

#include <util/protocol/http/HttpClient.h>

namespace trip
{
    namespace client
    {

        class Resource;

        class ResourceMapping
        {
        public:
            ResourceMapping(
                 boost::asio::io_service & io_svc);

            ~ResourceMapping();

        public:
            void find(
                Resource & resource);
            
            bool close();

        private:
            void handle_fetch(
                boost::system::error_code ec, 
                Resource & resource);

        private:
            util::protocol::HttpClient http_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_RESOURCE_MAPPING_H_
