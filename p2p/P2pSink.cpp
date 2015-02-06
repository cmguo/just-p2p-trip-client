// P2pSink.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSink.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pSink::P2pSink(
            Resource & resource, 
            UdpTunnel & tunnel)
            : Sink(resource)
            , UdpSession(tunnel)
        {
            MessageResponseBind rsp_bind;
            rsp_bind.sid = id();
            push(new Message(rsp_bind));
        }

        P2pSink::~P2pSink()
        {
        }

        bool P2pSink::response(
            boost::uint64_t index,
            Piece::pointer piece)
        {
            Message * msg = new Message;
            MessageResponseData & rsp_data(
                msg->as<MessageResponseData>());
            rsp_data.index = index;
            rsp_data.piece = piece;
            return push(msg);
        }

        void P2pSink::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Data:
                break;
            case REQ_Map:
                break;
            case REQ_Meta:
                break;
            case REQ_Unbind:
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
