// UdpEndpoint.h

#ifndef _TRIP_UDP_UDP_ENDPOINT_H_
#define _TRIP_UDP_UDP_ENDPOINT_H_

#include <framework/network/Endpoint.h>

namespace trip
{
    namespace client
    {

        struct Endpoint
        {
            enum Type {
                local, 
                nat, 
                turn, 
                reflex, 
            };
            boost::uint32_t type;
            framework::network::Endpoint endp;
            Endpoint() : type(local) {}
        };

        struct UdpEndpoint
        {
            Uuid id;
            std::vector<Endpoint> endpoints;

            UdpEndpoint()
            {
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_ENDPOINT_H_
