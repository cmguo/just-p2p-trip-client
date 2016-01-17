// Serialize.h

#ifndef _TRIP_CLIENT_MAIN_SERIALIZE_H_
#define _TRIP_CLIENT_MAIN_SERIALIZE_H_

#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Serialize.h"

#include <util/serialization/stl/map.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceManager & t)
        {
            ar & t.resources();
        }

    }
}

namespace util
{
    namespace serialization
    {

        template <class Archive>
        struct is_single_unit<Archive, trip::client::ResourceManager>
            : boost::true_type
        {
        };

    }
}

#endif // _TRIP_CLIENT_MAIN_SERIALIZE_H_
