// P2pHttpFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pHttpFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpPeer.h"
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
            head.host = "track.trip.com";
        }

        P2pHttpFinder::~P2pHttpFinder()
        {
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
            &format_msg<MessageRequestRegister>, 
            &format_msg<MessageRequestUnregister>, 
            &format_msg<MessageRequestFind>, 
        };

        static bool (* parse_msgs[]) (
            std::istream & is, 
            Message & msg) = {
            &parse_msg<MessageResponseRegister>, 
            &parse_msg<MessageResponseUnregister>, 
            &parse_msg<MessageResponseFind>, 
        };

        static char const * const commands[] = {
            "/register", 
            "/unregister", 
            "/find"
        };

        void P2pHttpFinder::send_msg(
            Message const & msg)
        {
            boost::uint16_t index = msg.type - REQ_Register;
            LOG_INFO("[send_msg] type=" << commands[index]);
            req_.head().path = commands[index];
            std::ostream os(&req_.data());
            format_msgs[index](os, msg);
            requests_.push_back(index);
            http_.async_fetch(req_, 
                boost::bind(&P2pHttpFinder::handle_fetch, this, _1));
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
