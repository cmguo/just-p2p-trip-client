// UdpEndpoint.h

#ifndef _TRIP_UDP_UDP_ENDPOINT_H_
#define _TRIP_UDP_UDP_ENDPOINT_H_

#include <framework/network/Endpoint.h>

namespace trip
{
    namespace client
    {

        using framework::network::Endpoint;

        struct UdpEndpoint
        {
            Uuid id;
            std::vector<Endpoint> endpoints;

            UdpEndpoint()
            {
            }
        };


        typedef std::pair<Endpoint, Endpoint> UdpEndpointPair;
        typedef std::vector<UdpEndpointPair> UdpEndpointPairs;
        
    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_ENDPOINT_H_
