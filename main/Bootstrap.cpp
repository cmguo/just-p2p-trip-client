// Bootstrap.cpp

#include "trip/client/Common.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/proto/MessageBootstrap.h"

#include <util/archive/XmlIArchive.h>

namespace trip
{
    namespace client
    {

        Bootstrap::Bootstrap(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<Bootstrap>(daemon, "Bootstrap")
            , http_(io_svc())
        {
        }

        Bootstrap::~Bootstrap()
        {
        }

        bool Bootstrap::startup(
            boost::system::error_code & ec)
        {
            Url url("http://boot.trip.com/bootstrap.xml");
            http_.async_fetch(url, 
                boost::bind(&Bootstrap::handle_fetch, this, _1));
            return true;
        }

        bool Bootstrap::shutdown(
            boost::system::error_code & ec)
        {
            http_.cancel_forever(ec);
            return !ec;
        }

        bool Bootstrap::get(
            boost::asio::io_service & io_svc, 
            std::string const & name,
            Url & url)
        {
            Bootstrap & bs(util::daemon::use_module<Bootstrap>(io_svc));
            return bs.get("index", url);
        }

        bool Bootstrap::get(
            std::string const & name,
            Url & url)
        {
            std::map<std::string, Url>::const_iterator iter = urls_.find(name);
            if (iter == urls_.end())
                return false;
            url = iter->second;
            return true;
        }

        void Bootstrap::handle_fetch(
            boost::system::error_code ec)
        {
            if (ec)
                return;

            util::archive::XmlIArchive<> ia(http_.response_data());
            ia >> urls_;
        }

    } // namespace client
} // namespace trip
