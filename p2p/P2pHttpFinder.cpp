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
            if (!ia)
                LOG_WARN("[parse_msg] failed, path=" << ia.failed_item_path());
            return !!ia;
        }

        static bool (* format_msgs[]) (
            std::ostream & os, 
            Message const & msg) = {
            &format_msg<MessageRequestLogin>, 
            &format_msg<MessageRequestSync>, 
            &format_msg<MessageRequestLogout>, 
            &format_msg<MessageRequestFind>, 
        };

        static bool (* parse_msgs[]) (
            std::istream & is, 
            Message & msg) = {
            &parse_msg<MessageResponseLogin>, 
            &parse_msg<MessageResponseSync>, 
            &parse_msg<MessageResponseLogout>, 
            &parse_msg<MessageResponseFind>, 
        };

        static char const * const commands[] = {
            "login", 
            "sync", 
            "logout", 
            "find"
        };

        void P2pHttpFinder::send_msg(
            Message const & msg)
        {
            Url url(urls_.at(0));
            boost::uint16_t index = msg.type - REQ_Sync;
            url.path(url.path() + commands[index]);
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
