// MessageData.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_DATA_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_DATA_H_

#include "trip/client/proto/MessageTraits.h"

#include <util/protocol/MessageData.h>

namespace trip
{
    namespace client
    {

        enum MessageType
        {
            // Tunnel Messages
 
            REQ_Probe       = 0, 
            REQ_Connect     = 1, 
            REQ_Ping        = 2, // RTT
            REQ_Disconnect  = 3, 

            RSP_Probe       = 0x0100 | REQ_Probe, 
            RSP_Connect     = 0x0100 | REQ_Connect, 
            RSP_Ping        = 0x0100 | REQ_Ping,
            RSP_Disconnect  = 0x0100 | REQ_Disconnect,

            // Session Messages
 
            REQ_Bind        = 16, // Bind a resource, create a session
            REQ_Meta        = 17, 
            REQ_Map         = 18, 
            REQ_Data        = 19, 
            REQ_Unbind      = 20, 

            NTF_Map         = 24, // Notify map changed

            RSP_Bind        = 0x0100 | REQ_Bind,
            RSP_Meta        = 0x0100 | REQ_Meta,
            RSP_Map         = 0x0100 | REQ_Map,
            RSP_Data        = 0x0100 | REQ_Data,
            RSP_Unbind      = 0x0100 | REQ_Unbind,

            // Tracker Messages
 
            REQ_Register    = 32, 
            REQ_Unregister  = 33, 
            REQ_Find        = 34, 

            RSP_Register    = 0x0100 | REQ_Register, 
            RSP_Unregister  = 0x0100 | REQ_Unregister, 
            RSP_Find        = 0x0100 | REQ_Find, 

        };

        template <
            typename T, 
            boost::uint16_t id
        >
        struct MessageData
            : util::protocol::MessageData<MessageTraits, T, id>
        {
        };

        extern char const * msg_type_str(
            boost::uint16_t type);

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_DATA_H_
