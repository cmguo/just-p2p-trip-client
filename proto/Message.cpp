// Message.cpp

#include "trip/client/Common.h"
#include "trip/client/proto/Message.h"
#include "trip/client/core/Memory.h"

#include <util/protocol/Message.hpp>

#include <framework/memory/BigFixedPool.h>
#include <framework/memory/PrivateMemory.h>

namespace trip
{
    namespace client
    {

        static framework::memory::BigFixedPool pool(framework::memory::PrivateMemory(), 1024 * 1024, sizeof(Message));

        Message * alloc_message()
        {
            void * ptr = Memory::inst().oom_alloc(pool, sizeof(Message));
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

        framework::memory::MemoryPool & message_pool()
        {
            return pool;
        }

        static char const * const msg_type_str1[][16] = {
            {"REQ_Connect", "REQ_Ping", "REQ_Disconnect"}, 
            {"REQ_Bind", "REQ_Meta", "REQ_Map", "REQ_Data", "REQ_Unbind", NULL, NULL, NULL, NULL, "NTF_Map"}, 
            {"REQ_Login", "REQ_Sync", "REQ_Logout", NULL, NULL, NULL, NULL, NULL, "REQ_Find"},
            {"REQ_Port", "REQ_Pass"},
        };

        static char const * const msg_type_str2[][16] = {
            {"RSP_Connect", "RSP_Ping", "RSP_Disconnect"}, 
            {"RSP_Bind", "RSP_Meta", "RSP_Map", "RSP_Data", "RSP_Unbind"}, 
            {"RSP_Login", "RSP_Sync", "RSP_Logout", NULL, NULL, NULL, NULL, NULL, "RSP_Find"},
            {"RSP_Port", "RSP_Pass"},
        };

        char const * msg_type_str(
            boost::uint16_t type)
        {
            char const * const (* msg_type_str)[16] = (type >> 8) ? msg_type_str2 : msg_type_str1;
            return msg_type_str[(type >> 4) & 0xf][type & 0xf];
        }

    } // namespace client
} // namespace trip
