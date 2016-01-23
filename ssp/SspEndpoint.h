// SspEndpoint.h

#ifndef _TRIP_SSP_SSP_ENDPOINT_H_
#define _TRIP_SSP_SSP_ENDPOINT_H_

#include <framework/network/Endpoint.h>

namespace trip
{
    namespace client
    {

        using framework::network::Endpoint;

        struct SspEndpoint
        {
            Uuid id;
            Endpoint endpoint;

            SspEndpoint()
            {
            }
        };


        typedef std::pair<Endpoint, Endpoint> SspEndpointPair;
        typedef std::vector<SspEndpointPair> SspEndpointPairs;
        
    } // namespace client
} // namespace trip

#endif // _TRIP_SSP_SSP_ENDPOINT_H_
