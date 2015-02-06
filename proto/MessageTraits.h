// MessageTraits.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_TRAITS_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_TRAITS_H_

#include <util/protocol/MessageTraits.h>
#include <util/archive/BigEndianBinaryIArchive.h>
#include <util/archive/BigEndianBinaryOArchive.h>

namespace trip
{
    namespace client
    {

        class MessageHeader;

        struct MessageTraits
            : util::protocol::MessageTraits<MessageTraits>
        {
            typedef boost::uint16_t id_type;

            typedef MessageHeader header_type;

            typedef util::archive::BigEndianBinaryIArchive<boost::uint8_t> i_archive_t;

            typedef util::archive::BigEndianBinaryOArchive<boost::uint8_t> o_archive_t;

            static size_t const max_size = 200;
        }; 

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_TRAITS_H_
