// Serialize.h

#ifndef _TRIP_CLIENT_MAIN_SERIALIZE_H_
#define _TRIP_CLIENT_MAIN_SERIALIZE_H_

#include "trip/client/main/Bootstrap.h"
#include "trip/client/main/MemoryManager.h"
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
            Bootstrap & t)
        {
            ar & SERIALIZATION_NVP_3(t, url)
                & SERIALIZATION_NVP_3(t, urls);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceManager & t)
        {
            ar & t.resources();
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            MemoryManager::Pool & t)
        {
            std::string name(t.name);
            ar & SERIALIZATION_NVP(name);
            framework::memory::MemoryPool const & p(*t.pool);
            ar & SERIALIZATION_NVP_2(p, capacity);
            ar & SERIALIZATION_NVP_2(p, consumption);
            ar & SERIALIZATION_NVP_2(p, peek);
            ar & SERIALIZATION_NVP_2(p, num_block);
            ar & SERIALIZATION_NVP_2(p, num_object);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            MemoryManager & t)
        {
            ar & t.pools();
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

        template <class Archive>
        struct is_single_unit<Archive, trip::client::MemoryManager>
            : boost::true_type
        {
        };

    }
}

#endif // _TRIP_CLIENT_MAIN_SERIALIZE_H_
