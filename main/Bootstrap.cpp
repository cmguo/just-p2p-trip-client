// Bootstrap.cpp

#include "trip/client/Common.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/proto/MessageBootstrap.h"

#include <util/archive/XmlIArchive.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Bootstrap", framework::logger::Debug);

        Bootstrap::Bootstrap(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<Bootstrap>(daemon, "trip.client.Bootstrap")
            , url_("http://boot.trip.com/trip/bootstrap.xml")
            , http_(io_svc())
        {
            module_config()
                << CONFIG_PARAM_NAME_NOACC("url", url_)
                ;
        }

        Bootstrap::~Bootstrap()
        {
        }

        bool Bootstrap::startup(
            boost::system::error_code & ec)
        {
            http_.async_fetch(url_, 
                boost::bind(&Bootstrap::handle_fetch, this, _1));
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

            util::archive::XmlIArchive<> ia(http_.response_data());
            ia >> urls_;
            raise(ready);
        }

    } // namespace client
} // namespace trip
