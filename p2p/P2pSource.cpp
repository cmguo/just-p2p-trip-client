// P2pSource.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pListener.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pSource::P2pSource(
            Resource & resource, 
            UdpTunnel & tunnel)
            : Source(resource)
            , UdpSession(tunnel)
        {
            MessageRequestBind req;
            req.rid = resource.id();
            req.sid = id();
            push(new Message(req));
        }

        P2pSource::~P2pSource()
        {
        }

        bool P2pSource::request(
            std::vector<boost::uint64_t> & pieces)
        {
            return false;
        }

        void P2pSource::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case RSP_Data:
                break;
            case RSP_Map:
                break;
            case RSP_Meta:
                break;
            case RSP_Bind:
                active();
                break;
            case RSP_Unbind:
                inactive();
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
