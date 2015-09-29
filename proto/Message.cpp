// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/proto/Message.h"

#include <framework/memory/BigFixedPool.h>
#include <framework/memory/PrivateMemory.h>

namespace trip
{
    namespace client
    {

        static framework::memory::BigFixedPool pool(framework::memory::PrivateMemory(), 1024 * 1024, sizeof(Message));

        Message * alloc_message()
        {
            void * ptr = pool.alloc(sizeof(Message));
            if (ptr) {
                return new (ptr) Message;
            } else {
                assert(false);
                return NULL;
            }
        }

        void free_message(
            Message * p)
        {
            p->~Message();
            pool.free(p);
        }

        static char const * const msg_type_str1[][16] = {
            {"REQ_Probe", "REQ_Connect", "REQ_Ping", "REQ_Disconnect"}, 
            {"REQ_Bind", "REQ_Meta", "REQ_Map", "REQ_Data", "REQ_Unbind"}, 
            {"REQ_Register", "REQ_Unregister", "REQ_Find"},
            {"RSP_Probe", "RSP_Connect", "RSP_Ping", "RSP_Disconnect"}, 
            {"RSP_Bind", "RSP_Meta", "RSP_Map", "RSP_Data", "RSP_Unbind"}, 
            {"RSP_Register", "RSP_Unregister", "RSP_Find"},
        };

        static size_t msg_type_str2[][16] = {
            {0, 1, 8, 2}, 
            {3, 4, 8, 5}, 
        };

        char const * msg_type_str(
            boost::uint16_t type)
        {
            size_t s = msg_type_str2[(type >> 8) & 1][(type >> 4) & 15];
            return s < 6 ? msg_type_str1[s][type & 15] : NULL;
        }

    } // namespace client
} // namespace trip
