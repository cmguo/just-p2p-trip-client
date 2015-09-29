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
            boost::uint16_t num_ep;
            Endpoint endpoints[4];

            UdpEndpoint()
                : num_ep(0)
            {
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_ENDPOINT_H_
