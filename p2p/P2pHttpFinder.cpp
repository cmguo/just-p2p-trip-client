// P2pHttpFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pHttpFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/core/Resource.h"

#include <util/archive/XmlIArchive.h>
#include <util/archive/XmlOArchive.h>
#include <util/protocol/Message.hpp>
#include <util/protocol/MessageHelper.h>
using util::protocol::HttpRequestHead;
using util::protocol::http_field::Connection;
using namespace util::archive;

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.P2pHttpFinder", framework::logger::Debug);

        P2pHttpFinder::P2pHttpFinder(
            P2pManager & manager)
            : P2pFinder(manager)
            , http_(manager.io_svc())
        {
            HttpRequestHead & head = req_.head();
            head.method = head.post;
            head.connection = Connection::keep_alive;
        }

        P2pHttpFinder::~P2pHttpFinder()
        {
        }

        void P2pHttpFinder::init()
        {
            Bootstrap::instance(http_.get_io_service()).get("tracker", urls_);
            if (urls_.empty())
                urls_.push_back(Url("http://tracker.trip.com/tracker/"));
        }

        template <class T>
        static bool format_msg(
            std::ostream & os, 
            Message const & msg)
        {
            T const & t = msg.as<T>();
            XmlOArchive<> oa(os);
            oa << t;
            return !!oa;

        }
 
        template <class T>
        static bool parse_msg(
            std::istream & is, 
            Message & msg)
        {
            T & t = msg.get<T>();
            XmlIArchive<> ia(is);
            ia >> t;
            return !!ia;
        }

        static bool (* format_msgs[]) (
            std::ostream & os, 
            Message const & msg) = {
            &format_msg<MessageRequestLogin>, 
            &format_msg<MessageRequestSync>, 
            &format_msg<MessageRequestLogout>, 
            NULL, NULL, NULL, NULL, NULL,
            &format_msg<MessageRequestFind>, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
            // Stun
            &format_msg<MessageRequestPort>, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
            &format_msg<MessageRequestPass>, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
        };

        static bool (* parse_msgs[]) (
            std::istream & is, 
            Message & msg) = {
            &parse_msg<MessageResponseLogin>, 
            &parse_msg<MessageResponseSync>, 
            &parse_msg<MessageResponseLogout>, 
            NULL, NULL, NULL, NULL, NULL, 
            &parse_msg<MessageResponseFind>, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
            // Stun
            &parse_msg<MessageResponsePort>, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            &parse_msg<MessageResponsePass>, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

        static char const * const commands[] = {
            "login", 
            "sync", 
            "logout", 
            NULL, NULL, NULL, NULL, NULL, 
            "find", 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
            "port", 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            "pass", 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

        void P2pHttpFinder::send_msg(
            Message const & msg)
        {
            Url url(urls_.at(0));
            boost::uint16_t index = msg.type - REQ_Login;
            url.path(url.path() + commands[index] + ".xml");
            req_.head().path = url.path_all();
            req_.head().host = url.host_svc();
            std::ostream os(&req_.data());
            format_msgs[index](os, msg);
            requests_.push_back(index);
            LOG_INFO("[send_msg] type=" << commands[index]);
            http_.async_fetch(req_, 
                boost::bind(&P2pHttpFinder::handle_fetch, this, _1));
        }

        boost::uint16_t P2pHttpFinder::get_id()
        {
            return 0;
        }

        void P2pHttpFinder::set_id(
            boost::uint16_t id)
        {
        }

        void P2pHttpFinder::handle_fetch(
            boost::system::error_code ec)
        {
            LOG_INFO("[handle_fetch] ec=" << ec.message());
            Message msg;
            if (!ec) {
                std::istream is(&http_.response_data());
                parse_msgs[requests_[0]](is, msg);
            }
            requests_.pop_front();
            http_.close(ec);
            handle_msg(msg);
        }

    } // namespace client
} // namespace trip
