// TunnelHeader.h

#ifndef _TRIP_CLIENT_PROTO_TUNNEL_HEADER_H_
#define _TRIP_CLIENT_PROTO_TUNNEL_HEADER_H_

#include <util/archive/BigEndianBinaryIArchive.h>
#include <util/archive/BigEndianBinaryOArchive.h>

namespace trip
{
    namespace client
    {

        typedef util::archive::BigEndianBinaryIArchive<boost::uint8_t> TunnelIArchive;

        typedef util::archive::BigEndianBinaryOArchive<boost::uint8_t> TunnelOArchive;

        class TunnelHeader
        {
        public:
            boost::uint16_t ver;    // version
            boost::uint16_t tid;    // tunnel id
            boost::uint16_t seq;    // sequence in tunnel
            boost::uint16_t chk;    // check sum

        public:
            TunnelHeader()
                : ver(0)
                , tid(0)
                , seq(0)
                , chk(0)
            {
            }

        public:
            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & ver
                    & tid
                    & seq
                    & chk;
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_TUNNEL_HEADER_H_
