// MessageIndex.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_BOOTSTRAP_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_BOOTSTRAP_H_

#include "trip/client/core/Resource.h"
#include "trip/client/utils/Serialize.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/map.h>

namespace trip
{
    namespace client
    {

        struct BootUrls
        {
            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_BOOTSTRAP_H_
