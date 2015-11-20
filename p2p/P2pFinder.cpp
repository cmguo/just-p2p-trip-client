// P2pFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pFinder::P2pFinder(
            P2pManager & manager)
            : pmgr_(manager)
            , umgr_(util::daemon::use_module<UdpManager>(pmgr_.io_svc()))
            , inited_(false)
        {
            Bootstrap::instance(manager.io_svc()).ready.on(
                boost::bind(&P2pFinder::on_event, this));
        }

        P2pFinder::~P2pFinder()
        {
        }

        void P2pFinder::open()
        {
            if (!inited_)
                return;

            MessageRequestLogin login;
            login.sid = get_id();
            login.endpoint = umgr_.local_endpoint();
            send_msg(login);

            MessageRequestSync sync;
            sync.type = 0; // full
            //sync.rids;
            send_msg(sync);
        }

        void P2pFinder::close()
        {
            MessageRequestLogout logout;
            send_msg(logout);
        }

        std::string P2pFinder::proto() const
        {
            return "p2p";
        }

        void P2pFinder::on_event()
        {
            init();
            inited_ = true;
            open();
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
            UdpTunnel * tunnel = umgr_.get_tunnel(pid);
            assert(tunnel);
            return new P2pSource(resource, *tunnel, url);
        }

        bool P2pFinder::handle_msg(
            Message const & msg)
        {
            switch (msg.type) {
            case RSP_Login:
                break;
            case RSP_Sync:
                break;
            case RSP_Logout:
                break;
            case RSP_Find:
                handle_find(msg.as<MessageResponseFind>());
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
                umgr_.get_tunnel(endpoint);
                url.param("pid", format(endpoint.id));
                urls.push_back(url);
            }
            found(find.rid, urls);
        }

    } // namespace client
} // namespace trip
