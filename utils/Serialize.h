// Serialize.h

#ifndef _TRIP_CLIENT_UTILS_SERIALIZE_H_
#define _TRIP_CLIENT_UTILS_SERIALIZE_H_

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include <util/serialization/Collection.h>
#include <util/serialization/Uuid.h>

namespace boost
{

    template <typename Archive>
    void load(
        Archive & ar, 
        dynamic_bitset<uint32_t> & map)
    {
        using util::serialization::make_sized;

        uint32_t count;
        ar >> count;
        uint32_t count2 = count / sizeof(uint32_t);
        if (count & (sizeof(uint32_t) - 1))
            ++count2;
        std::vector<uint32_t> data;
        ar >> make_sized(data, count2);
        if (ar) {
            map.clear();
            from_block_range(data.begin(), data.end(), map);
        }
    }

    template <typename Archive>
    void save(
        Archive & ar, 
        dynamic_bitset<uint32_t> const & map)
    {
        using util::serialization::make_sized;

        uint32_t count = map.size();
        uint32_t count2 = count / sizeof(uint32_t);
        if (count & (sizeof(uint32_t) - 1))
            ++count2;
        std::vector<uint32_t> data(0, count2);
        to_block_range(map, data.begin());
        ar << count 
            << make_sized(data, count2);
    }

    SERIALIZATION_SPLIT_FREE(dynamic_bitset<uint32_t>);

} // namespace boost

#endif // _TRIP_CLIENT_UTILS_SERIALIZE_H_

