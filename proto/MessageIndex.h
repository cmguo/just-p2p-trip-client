// MessageIndex.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_INDEX_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_INDEX_H_

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/vector.h>

namespace framework
{
    namespace string
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Url & url)
        {
            std::string str = url.to_string();
            ar & str;
            url.from_string(str);
        }

    }
}

namespace trip
{
    namespace client
    {

        struct ResourceInfo
        {
            Uuid rid;
            boost::uint32_t interval;
            boost::uint64_t duration;
            std::vector<Url> urls;
            
            ResourceInfo()
                : interval(0)
                , duration(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                std::string rid = this->rid.to_string();
                ar & SERIALIZATION_NVP(rid)
                    & SERIALIZATION_NVP(interval)
                    & SERIALIZATION_NVP(duration)
                    & SERIALIZATION_NVP(urls);
                if (ar)
                    this->rid.from_string(rid);
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_INDEX_H_
