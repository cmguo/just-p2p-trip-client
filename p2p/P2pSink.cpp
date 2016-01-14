// P2pSink.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pSink.h"
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
            Resource & resource, 
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
            , resource_(resource)
        {
        }

        P2pSink::~P2pSink()
        {
        }

        UdpSession * P2pSink::create_session(
            ResourceManager & rmgr, 
            UdpTunnel & tunnel, 
            Message & msg)
        {
            MessageRequestBind const & req(msg.as<MessageRequestBind>());
            Resource * res = rmgr.find(req.rid);
            P2pSink * sink = NULL;
            if (res == NULL) {
                msg.sid = req.sid;
                MessageResponseBind & resp = 
                    msg.get<MessageResponseBind>();
                resp.sid = 0;
                tunnel.push(&msg);
            } else {
                sink = new P2pSink(*res, tunnel);
                sink->p_id(req.sid);
                MessageResponseBind & resp = 
                    msg.get<MessageResponseBind>();
                resp.sid = sink->l_id();
                sink->send_msg(&msg);
            }
            return sink;
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
                        LOG_DEBUG("[on_msg] index=" << req.index);
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
                        LOG_DEBUG("[on_msg] index=" << req.index);
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
            case REQ_Unbind:
                {
                    free_message(msg);
                }
                break;
            default:
                free_message(msg);
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
