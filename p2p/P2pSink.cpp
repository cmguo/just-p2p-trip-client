// P2pSink.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSink.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.P2pSink", framework::logger::Debug);

        P2pSink::P2pSink(
            P2pManager & manager, 
            Resource & resource, 
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
            , manager_(manager)
            , resource_(resource)
        {
        }

        P2pSink::~P2pSink()
        {
            manager_.del_sink(this);
        }

        Uuid const & P2pSink::get_bind_rid(
            Message & msg)
        {
            MessageRequestBind const & req(msg.as<MessageRequestBind>());
            return req.rid;
        }

        void P2pSink::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case REQ_Data:
                {
                    MessageRequestData req =
                        msg->as<MessageRequestData>();
                    Piece::pointer piece = resource_.get_piece(req.index, 1);
                    if (piece) {
                        //LOG_DEBUG("[on_msg] index=" << req.index);
                        MessageResponseData & resp(
                            msg->get<MessageResponseData>());
                        resp.index = req.index;
                        resp.piece = piece;
                        msg->reset(resp);
                        send_msg(msg);
                    } else {
                        free_message(msg);
                    }
                    for (size_t i = 0; i < req.offsets.size(); ++i) {
                        req.index += req.offsets[i];
                        //LOG_DEBUG("[on_msg] index[" << i << "]=" << req.index);
                        Piece::pointer piece = resource_.get_piece(req.index, 1);
                        if (piece) {
                            msg = alloc_message();
                            MessageResponseData & resp(
                                msg->get<MessageResponseData>());
                            resp.index = req.index;
                            resp.piece = piece;
                            send_msg(msg);
                        } else {
                            assert(false);
                        }
                    }
                }
                break;
            case REQ_Map:
                {
                    MessageRequestMap req =
                        msg->as<MessageRequestMap>();
                    MessageResponseMap & resp(
                        msg->get<MessageResponseMap>());
                    resp.start = req.start;
                    resource_.get_segment_map(req.start, req.count, resp.map);
                    send_msg(msg);
                }
                break;
            case REQ_Meta:
                {
                    free_message(msg);
                }
                break;
            case REQ_Bind:
                {
                    MessageRequestBind req =
                        msg->as<MessageRequestBind>();
                    MessageResponseBind & resp = 
                        msg->get<MessageResponseBind>();
		    p_id(req.sid);
                    resp.sid = l_id();
                    send_msg(msg);
                }
                break;
            case REQ_Unbind:
                {
                    free_message(msg);
                }
                break;
            default:
                free_message(msg);
                //assert(false);
            }
        }

        void P2pSink::on_tunnel_disconnect()
        {
            UdpSession::on_tunnel_disconnect();
            delete this;
        }

    } // namespace client
} // namespace trip
