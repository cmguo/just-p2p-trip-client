// Serialize.h

#ifndef _TRIP_CLIENT_UTILS_SERIALIZE_H_
#define _TRIP_CLIENT_UTILS_SERIALIZE_H_

#include <util/serialization/Collection.h>
#include <util/serialization/Uuid.h>
#include <util/serialization/Array.h>

#include <framework/network/Endpoint.h>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

namespace framework
{
    namespace network
    {

        SERIALIZATION_SPLIT_FREE(Endpoint);

        template <typename Archive>
        void save(
            Archive & ar, 
            Endpoint const & ep)
        {
            boost::uint8_t protocol = ep.protocol();
            boost::uint8_t family = ep.family();
            boost::uint16_t port = ep.port();
            ar << protocol << family << port;
            if (family == ep.v4) {
                boost::uint32_t ipv4 = ep.ip_v4();
                ar << ipv4;
            } else {
                Endpoint::ip_v6_bytes_type ipv6 = ep.ip_v6();
                ar << framework::container::make_array(ipv6.elems);
            }
        }

        template <typename Archive>
        void load(
            Archive & ar, 
            Endpoint & ep)
        {
            std::string str;
            ar >> str;
            ep.from_string(str);
        }

    }
}

namespace boost
{

    template <typename Archive>
    void load(
        Archive & ar, 
        dynamic_bitset<boost::uint32_t> & map)
    {
        using util::serialization::make_sized;

        boost::uint32_t count;
        ar >> count;
        boost::uint32_t count2 = count / sizeof(boost::uint32_t);
        if (count & (sizeof(boost::uint32_t) - 1))
            ++count2;
        std::vector<boost::uint32_t> data;
        ar >> make_sized(data, count2);
        if (ar) {
            map.resize(count);
            from_block_range(data.begin(), data.end(), map);
        }
    }

    template <typename Archive>
    void save(
        Archive & ar, 
        dynamic_bitset<boost::uint32_t> const & map)
    {
        using util::serialization::make_sized;

        boost::uint32_t count = map.size();
        boost::uint32_t count2 = count / sizeof(boost::uint32_t);
        if (count & (sizeof(boost::uint32_t) - 1))
            ++count2;
        std::vector<boost::uint32_t> data(count2, 0);
        to_block_range(map, data.begin());
        ar << count 
            << make_sized(data, count2);
    }

    SERIALIZATION_SPLIT_FREE(dynamic_bitset<boost::uint32_t>);

} // namespace boost

#endif // _TRIP_CLIENT_UTILS_SERIALIZE_H_

