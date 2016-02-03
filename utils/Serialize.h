// Serialize.h

#ifndef _TRIP_CLIENT_UTILS_SERIALIZE_H_
#define _TRIP_CLIENT_UTILS_SERIALIZE_H_

#include <util/serialization/Collection.h>
#include <util/serialization/Uuid.h>
#include <util/serialization/Array.h>

#include <framework/network/Endpoint.h>
#include <framework/timer/ClockTime.h>
#include <framework/system/HumanNumber.h>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <boost/filesystem/path.hpp>

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
            boost::uint8_t protocol = ep.type() | ep.protocol();
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
            boost::uint8_t protocol = 0;
            boost::uint8_t family = 0;
            boost::uint16_t port = 0;
            ar >> protocol >> family >> port;
            if (!ar) return;
            if (family == ep.v4) {
                boost::uint32_t ipv4 = 0;
                ar >> ipv4;
                if (!ar) return;
                ep.ip_v4(ipv4);
            } else {
                Endpoint::ip_v6_bytes_type ipv6;
                ar >> framework::container::make_array(ipv6.elems);
                if (!ar) return;
                ep.ip_v6(ipv6);
            }
            ep.protocol(Endpoint::ProtocolEnum(protocol));
            ep.family(Endpoint::FamilyEnum(family));
            ep.port(port);
        }

    }

    namespace timer
    {

        inline void from_string(
            std::string const & str, 
            Time & t)
        {
            Duration d;
            if (d.from_string(str))
                t = time_launch + d;
        }

        inline std::string  to_string(
            Time const & t)
        {
            return (t - time_launch).to_string();
        }

        template <typename Archive>
        void load(
            Archive & ar, 
            Time & t)
        {
            Duration d;
            ar >> d;
            if (ar)
                t = time_launch + d;
        }

        template <typename Archive>
        void save(
            Archive & ar, 
            Time const & t)
        {
            ar << (t - time_launch);
        }

        SERIALIZATION_SPLIT_FREE(Time);

        template <typename Archive>
        void load(
            Archive & ar, 
            Duration & t)
        {
            std::string str;
            ar >> str;
            if (ar)
                t.from_string(str);
        }

        template <typename Archive>
        void save(
            Archive & ar, 
            Duration const & t)
        {
            ar << t.to_string();
        }

        SERIALIZATION_SPLIT_FREE(Duration);
    }

    namespace system
    {

        template <typename Archive, typename _Ty>
        void load(
            Archive & ar, 
            HumanNumber<_Ty> & t)
        {
            _Ty d;
            ar >> d;
            if (ar)
                t = d;
        }

        template <typename Archive, typename _Ty>
        void save(
            Archive & ar, 
            HumanNumber<_Ty> const & t)
        {
            ar << t.value();;
        }

        template <typename Archive, typename _Ty>
        void serialize(
            Archive & ar, 
            HumanNumber<_Ty> & t)
        {
            util::serialization::split_free(ar, t);
        }

    } // namespace system

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
        boost::uint32_t count2 = count / 32;
        if (count & 31)
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
        boost::uint32_t count2 = count / 32;
        if (count & 31)
            ++count2;
        std::vector<boost::uint32_t> data(count2, 0);
        to_block_range(map, data.begin());
        ar << count 
            << make_sized(data, count2);
    }

    SERIALIZATION_SPLIT_FREE(dynamic_bitset<boost::uint32_t>);

    namespace filesystem
    {

        template <typename Archive>
        void load(
            Archive & ar, 
            path & p)
        {
            std::string str;
            ar >> str;
            if (ar) {
                p = str;
            }
        }

        template <typename Archive>
        void save(
            Archive & ar, 
            path const & p)
        {
            std::string str = p.string();
            ar << str;
        }

        SERIALIZATION_SPLIT_FREE(path);

    } // namespace filesystem
} // namespace boost

namespace util
{
    namespace serialization
    {

        template <class Archive>
        struct is_single_unit<Archive, boost::filesystem::path>
            : boost::true_type
        {
        };

        /*
        template <typename Archive, typename _Ty>
        struct is_single_unit<Archive, framework::system::HumanNumber<_Ty> >
            : boost::true_type
        {
        };
        */

        template <class Archive>
        struct is_single_unit<Archive, framework::timer::Time>
            : boost::true_type
        {
        };

    }
}

#endif // _TRIP_CLIENT_UTILS_SERIALIZE_H_

