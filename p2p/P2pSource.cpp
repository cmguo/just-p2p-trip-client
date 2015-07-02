// P2pSource.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pSource::P2pSource(
            Resource & resource,
            UdpTunnel & tunnel, 
            Url const & url)
            : Source(resource, url)
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

        bool P2pSource::open(
            Url const & url)
        {
            return false;
        }

        bool P2pSource::close()
        {
            return false;
        }

        bool P2pSource::do_request(
            std::vector<DataId> const & pieces)
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
                break;
            case RSP_Unbind:
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
