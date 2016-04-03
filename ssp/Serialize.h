// Serialize.h

#ifndef _TRIP_CLIENT_SSP_SERIALIZE_H_
#define _TRIP_CLIENT_SSP_SERIALIZE_H_

#include "trip/client/ssp/SspManager.h"
#include "trip/client/ssp/SspBus.h"
#include "trip/client/ssp/SspTunnel.h"
#include "trip/client/ssp/SspSession.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/net/Serialize.h"

#include <util/serialization/stl/vector.h>
#include <util/serialization/stl/map.h>
#include <util/serialization/Url.h>
#include <util/serialization/Uuid.h>

namespace trip
{
    namespace client
    {

        using util::serialization::make_sized;

        /* SspEndpoint */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SspEndpoint & t)
        {
            ar
                & SERIALIZATION_NVP_1(t, id)
                & SERIALIZATION_NVP_1(t, endpoint)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SspBus & t)
        {
            serialize(ar, (Bus &)t);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SspTunnel & t)
        {
            serialize(ar, (Bus &)t);
            ar & SERIALIZATION_NVP_2(t, endpoint);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SspManager & t)
        {
            ar
                & SERIALIZATION_NVP_2(t, local_endpoint)
                & SERIALIZATION_NVP_2(t, bus)
                & SERIALIZATION_NVP_2(t, tunnels)
                ;
        }

    }
}

#endif // _TRIP_CLIENT_SSP_SERIALIZE_H_
