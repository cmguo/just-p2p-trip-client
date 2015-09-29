// P2pFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pFinder::P2pFinder(
            P2pManager & manager)
            : pmgr_(manager)
        {
        }

        P2pFinder::~P2pFinder()
        {
        }

        void P2pFinder::open()
        {
            UdpEndpoint & endp(pmgr_.local_endpoint());
            MessageRequestRegister reg;
            reg.endpoint = endp;
            //reg.rids;
            send_msg(reg);
        }

        void P2pFinder::close()
        {
            MessageRequestUnregister unreg;
            unreg.pid = pmgr_.local_endpoint().id;
            send_msg(unreg);
        }

        std::string P2pFinder::proto() const
        {
            return "p2p";
        }

        void P2pFinder::find(
            Resource & resource, 
            size_t count)
        {
            MessageRequestFind find;
            find.rid = resource.id();
            find.count = (boost::uint16_t)count;
            send_msg(find);
        }

        Source * P2pFinder::create(
            Resource & resource, 
            Url const & url)
        {
            Uuid pid(url.param("pid"));
            UdpTunnel * tunnel = pmgr_.get_tunnel(pid);
            assert(tunnel);
            return new P2pSource(resource, *tunnel, url);
        }

        bool P2pFinder::handle_msg(
            Message const & msg)
        {
            switch (msg.type) {
            case RSP_Find:
                handle_find(msg.as<MessageResponseFind>());
                break;
            case RSP_Register:
                break;
            case RSP_Unregister:
                break;
            default:
                return false;
            }
            return true;
        }

        void P2pFinder::handle_find(
            MessageResponseFind const & find)
        {
            std::vector<Url> urls;
            Url url("p2p:///");
            for (size_t i = 0; i < find.endpoints.size(); ++i) {
                UdpEndpoint const & endpoint = find.endpoints[i];
                pmgr_.get_tunnel(endpoint);
                url.param("pid", format(endpoint.id));
                urls.push_back(url);
            }
            found(find.rid, urls);
        }

    } // namespace client
} // namespace trip
