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
 
            REQ_Connect     = 0x00, 
            REQ_Ping        = 0x01, // RTT
            REQ_Disconnect  = 0x02, 

            RSP_Connect     = 0x0100 | REQ_Connect, 
            RSP_Ping        = 0x0100 | REQ_Ping,
            RSP_Disconnect  = 0x0100 | REQ_Disconnect,

            // Session Messages
 
            REQ_Bind        = 0x10, // Bind a resource, create a session
            REQ_Meta        = 0x11, 
            REQ_Map         = 0x12, 
            REQ_Data        = 0x13, 
            REQ_Unbind      = 0x14, 

            NTF_Map         = 0x18, // Notify map changed

            RSP_Bind        = 0x0100 | REQ_Bind,
            RSP_Meta        = 0x0100 | REQ_Meta,
            RSP_Map         = 0x0100 | REQ_Map,
            RSP_Data        = 0x0100 | REQ_Data,
            RSP_Unbind      = 0x0100 | REQ_Unbind,

            // Tracker Messages
 
            REQ_Login       = 0x20, 
            REQ_Sync        = 0x21, 
            REQ_Logout      = 0x23, 

            REQ_Find        = 0x28, 

            RSP_Login       = 0x0100 | REQ_Login, 
            RSP_Sync        = 0x0100 | REQ_Sync, 
            RSP_Logout      = 0x0100 | REQ_Logout, 

            RSP_Find        = 0x0100 | REQ_Find, 

            // Stun Messages

            REQ_Port        = 0x30, // Alloc NAT Port, with help of tracker
            REQ_Pass        = 0x38, // Help pass through

            RSP_Port        = 0x0100 | REQ_Port, 
            RSP_Pass        = 0x0100 | REQ_Pass, 
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
