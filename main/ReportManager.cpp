// Dac.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ReportManager.h"
#include "trip/client/main/ReportInfo.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/main/Bootstrap.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

#include <util/protocol/pptv/Base64.h>
#include <util/protocol/http/HttpClient.h>
#include <util/archive/XmlIArchive.h>

#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/streambuf.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.ReportManager", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        ReportManager::ReportManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<ReportManager>(daemon, "trip.client.ReportManager")
            , http_client_(new util::protocol::HttpClient(io_svc()))
        {
            Bootstrap::instance(io_svc()).ready.on(
                boost::bind(&ReportManager::on_boot_ready, this));
        }

        ReportManager::~ReportManager()
        {
            if (http_client_) {
                delete http_client_;
                http_client_ = NULL;
            }
        }

        bool ReportManager::startup(
            error_code & ec)
        {
            return true;
        }

        bool ReportManager::shutdown(
            error_code & ec)
        {
            return true;
        }

        void ReportManager::on_boot_ready()
        {
            Url url("http://report.trip.com/report.xml");
            Bootstrap::instance(io_svc()).get("report", url);
            http_client_->async_fetch(url, 
                boost::bind(&ReportManager::handle_fetch_all, this, _1));
        }

        void ReportManager::handle_fetch_all(
            error_code const & ec)
        {
            if (ec) {
                LOG_WARN("[handle_fetch_all] error, ec: " << ec.message());
                return;
            }
            util::archive::XmlIArchive<> ia(http_client_->response_data());
            ia >> set_;
            if (!ia) {
                LOG_WARN("[handle_fetch_all] format error");
            }

            for (size_t i = 0; i < set_.infos.size(); ++i)
                submit(set_.infos[i]);
        }

        void ReportManager::report(
            std::string const & info)
        {
            std::vector<ReportInfo>::const_iterator iter = 
                std::find(set_.infos.begin(), set_.infos.end(), info);
            if (iter != set_.infos.end())
                submit(*iter);
        }

        void ReportManager::handle_fetch(
            error_code const & ec)
        {
            if (ec) {
                LOG_WARN("[handle_fetch] error, ec: " << ec.message());

                //std::string path = decrypt(http_client->get_request_head().path.substr(8));

                //LOG_STR(Trace, path.c_str());
            }

            tasks_.pop_front();

            if (!tasks_.empty()) {
                http_client_->async_fetch(tasks_.front(), 
                    boost::bind(&ReportManager::handle_fetch, this, _1));
            }
        }

        void ReportManager::submit2(
            ReportInfo const & info)
        {
            Url url(info.url);
            DataGraph & graph(DataGraph::get(io_svc()));
            for (size_t i = 0; i < info.params.size(); ++i) {
                std::ostringstream oss;
                graph.dump(info.params[i].path, oss);
                url.param(info.params[i].name, oss.str());
            }

            LOG_STR(framework::logger::Trace, ("submit2", url.to_string()));

            tasks_.push_back(url);

            if (tasks_.size() == 1) {
                http_client_->async_fetch(tasks_.front(), 
                    boost::bind(&ReportManager::handle_fetch, this, _1));
            }
        }

        void ReportManager::submit(
            ReportInfo const & info)
        {
            io_svc().post(boost::bind(&ReportManager::submit2, this, info));
        }

        /*
        std::string ReportManager::encrypt(
            std::string const & str)
        {
            return pptv::base64_encode(str, JUST_KEY);
        }

        std::string ReportManager::decrypt(
            std::string const & str)
        {
            return pptv::base64_decode(str, JUST_KEY);
        }
        */

    }
}
