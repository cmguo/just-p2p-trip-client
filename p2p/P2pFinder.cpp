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
            MessageRequestRegister reg;
            send_msg(reg);
        }

        void P2pFinder::close()
        {
            MessageRequestUnregister unreg;
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
            UdpTunnel & tunnel = pmgr_.get_tunnel(pid);
            return new P2pSource(resource, tunnel, url);
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
            for (size_t i = 0; i < find.peers.size(); ++i) {
                P2pPeer const & peer = find.peers[i];
                pmgr_.get_tunnel(peer);
                url.param("pid", format(peer.id));
                urls.push_back(url);
            }
            found(find.rid, urls);
        }

    } // namespace client
} // namespace trip
