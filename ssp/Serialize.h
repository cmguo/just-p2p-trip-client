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
            SspEndpoint & endpoint)
        {
            ar & SERIALIZATION_NVP_NAME("id", endpoint.id);
            ar & SERIALIZATION_NVP_NAME("endpoint", endpoint.endpoint);
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
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SspManager & t)
        {
            ar & SERIALIZATION_NVP_NAME("local_endpoint", t.local_endpoint());
            ar & SERIALIZATION_NVP_NAME("bus", t.bus());
            ar & SERIALIZATION_NVP_NAME("tunnels", t.tunnels());
        }

    }
}

#endif // _TRIP_CLIENT_SSP_SERIALIZE_H_
