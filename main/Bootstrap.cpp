// Bootstrap.cpp

#include "trip/client/Common.h"
#include "trip/client/Version.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/proto/MessageBootstrap.h"

#include <util/archive/XmlIArchive.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/FormatStl.h>
#include <framework/string/ParseStl.h>

#include <boost/type_traits/remove_const.hpp>

#include <fstream>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Bootstrap", framework::logger::Debug);

        Bootstrap::Bootstrap(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<Bootstrap>(daemon, "trip.client.Bootstrap")
            , util::event::Observable("trip.client.Bootstrap")
            , ready("ready")
            , url_("http://boot.trip.com/trip/bootstrap.xml")
            , http_(io_svc())
        {
            module_config()
                << CONFIG_PARAM_NAME_RDWR("url", url_)
                ;
            register_event(ready);
        }

        Bootstrap::~Bootstrap()
        {
        }

        bool Bootstrap::startup(
            boost::system::error_code & ec)
        {
            if (url_.protocol() == "file") {
                std::ifstream ifs(url_.path().c_str());
                std::ostream os(&http_.response_data());
                os << ifs.rdbuf();
                io_svc().post(boost::bind(&Bootstrap::handle_fetch, this, ec));
            } else {
                http_.async_fetch(url_, 
                    boost::bind(&Bootstrap::handle_fetch, this, _1));
            }
            return true;
        }

        bool Bootstrap::shutdown(
            boost::system::error_code & ec)
        {
            http_.cancel_forever(ec);
            return !ec;
        }

        Bootstrap & Bootstrap::instance(
            boost::asio::io_service & io_svc)
        {
            return util::daemon::use_module<Bootstrap>(io_svc);
        }

        bool Bootstrap::get(
            std::string const & name,
            Url & url)
        {
            std::map<std::string, std::vector<Url> >::const_iterator iter = urls_.find(name);
            if (iter == urls_.end() || iter->second.empty())
                return false;
            url = iter->second[0];
            return true;
        }

        bool Bootstrap::get(
            std::string const & name,
            std::vector<Url> & urls)
        {
            std::map<std::string, std::vector<Url> >::const_iterator iter = urls_.find(name);
            if (iter == urls_.end())
                return false;
            urls = iter->second;
            return true;
        }

        void Bootstrap::handle_fetch(
            boost::system::error_code ec)
        {
            if (ec) {
                LOG_WARN("[handle_fetch] ec=" << ec.message());
                return;
            }
            std::istream is(&http_.response_data());
            parse(is);
            raise(ready);
        }

        void Bootstrap::parse(
            std::istream & is)
        {
            util::archive::XmlIArchive<> ia(is);
            ia >> urls_;

            DataGraph const & dg = util::daemon::use_module<DataGraph>(get_daemon());
            UdpManager & udp = util::daemon::use_module<UdpManager >(get_daemon());
            framework::configure::ConfigModule & cfg = module_config();
            boost::system::error_code ec;
            std::map<std::string, std::vector<Url> >::iterator iter = urls_.begin();
            std::string uid = udp.local_endpoint().id.to_string();
            for (; iter != urls_.end(); ++iter) {
                std::vector<Url> & urls = iter->second;
                for (size_t i = 0; i < urls.size(); ++i) {
                    urls[i].param("client_id", uid);
                    urls[i].param("client_version", trip::client::version_string());
                    dg.expandUrl(urls[i], ec);
                }
                cfg << CONFIG_PARAM_NAME_RDONLY(iter->first, urls);
            }
        }

    } // namespace client
} // namespace trip
