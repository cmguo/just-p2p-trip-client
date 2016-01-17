// Message.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_H_

#include "trip/client/proto/MessageHeader.h"
#include "trip/client/proto/MessageTraits.h"

#include <util/protocol/Message.h>

namespace trip
{
    namespace client
    {

        typedef util::protocol::Message<MessageTraits> Message;

        Message * alloc_message();

        void free_message(Message * msg);

        char const * msg_type_str(
            boost::uint16_t type);

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_H_
