// P2pFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Resource.h"

#include <framework/string/Format.h>

namespace trip
{
    namespace client
    {

        P2pFinder::P2pFinder(
            P2pManager & manager)
            : pmgr_(manager)
            , umgr_(util::daemon::use_module<UdpManager>(pmgr_.io_svc()))
            , rmgr_(util::daemon::use_module<ResourceManager>(pmgr_.io_svc()))
            , inited_(false)
        {
            Bootstrap::instance(manager.io_svc()).ready.on(
                boost::bind(&P2pFinder::on_event, this, _1, _2));

            Message msg;
            msg.reset(MessageResponseLogin());
            msg.reset(MessageResponseSync());
            msg.reset(MessageResponseLogout());
            msg.reset(MessageResponseFind());
            //msg.reset(MessageResponsePort());
            //msg.reset(MessageResponsePass());
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

        void P2pFinder::sync(
            int type, 
            Resource * res)
        {
            MessageRequestSync sync;
            sync.type = type;
            if (type == 0) {
                std::map<Uuid, Resource *> const & resources(rmgr_.resources());
                std::map<Uuid, Resource *>::const_iterator iter = resources.begin();
                for (; iter != resources.end(); ++iter) {
                    sync.rids.push_back(iter->first);
                    if (sync.rids.size() == PIECE_SIZE / 16) {
                        send_msg(sync);
                        sync.type = 1;
                        sync.rids.clear();
                    }
                }
                if (!sync.rids.empty())
                    send_msg(sync);
                rmgr_.resource_added.on(
                    boost::bind(&P2pFinder::on_event, this, _1, _2));
                rmgr_.resource_deleting.on(
                    boost::bind(&P2pFinder::on_event, this, _1, _2));
            } else {
                sync.rids.push_back(res->id());
                send_msg(sync);
            }
        }

        void P2pFinder::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == Bootstrap::instance(pmgr_.io_svc())) {
                init();
                inited_ = true;
                open();
            } else if (observable == rmgr_) {
                if (event == rmgr_.resource_added) {
                    sync(1, rmgr_.resource_added.resource);
                } else {
                    sync(2, rmgr_.resource_added.resource);
                }
            }
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
                {
                    MessageResponseLogin const & resp =
                        msg.as<MessageResponseLogin>();
                    set_id(resp.sid);
                    sync(0, NULL);
                }
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
                Uuid const & lid(umgr_.local_endpoint().id);
                if (endpoint.id == lid)
                    continue;
                if (endpoint.endpoints.empty())
                    continue;
                umgr_.get_tunnel(endpoint);
                url.host(endpoint.endpoints[0].ip_str());
                url.svc(framework::string::format(endpoint.endpoints[0].port()));
                url.param("pid", format(endpoint.id));
                urls.push_back(url);
            }
            found(find.rid, urls);
        }

    } // namespace client
} // namespace trip
