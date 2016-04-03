// Dac.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ReportManager.h"
#include "trip/client/main/ReportInfo.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/main/Bootstrap.h"

#include <util/protocol/pptv/Base64.h>
#include <util/protocol/http/HttpClient.h>
#include <util/archive/XmlIArchive.h>
#include <util/event/Observable.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>
#include <framework/string/Slice.h>

#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/regex.hpp>

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
            mProcs["incremmental"] = ReportValueProc_incremmental::create;
        }

        ReportManager::~ReportManager()
        {
            if (http_client_) {
                delete http_client_;
                http_client_ = NULL;
            }
        }

        bool ReportManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool ReportManager::shutdown(
            boost::system::error_code & ec)
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
            boost::system::error_code const & ec)
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

            for (size_t i = 0; i < set_.infos.size(); ++i) {
                ReportInfo & info(set_.infos[i]);
                if (!info.event.empty()) {
                    // util.timer.TimerManager/t_1_s
                    std::string::size_type p = info.event.find('/');
                    if (p != std::string::npos) {
                        util::event::Observable & o(o.get(info.event.substr(0, p)));
                        o.on(info.event.substr(p + 1), 
                            boost::bind(&ReportManager::on_event, this, _1, _2, boost::cref(info)));
                    }
                }
                for (size_t j = 0; j < info.params.size(); ++j) {
                    ReportParam & param(info.params[j]);
                    if (param.procs.is_initialized()) {
                        std::string procs = param.procs.get();
                        std::vector<std::string> vec;
                        framework::string::slice<std::string>(procs, std::back_inserter(vec));
                        for (size_t k = 0; k < vec.size(); ++k) {
                            std::map<std::string, ReportValueProc::creater>::const_iterator iter = 
                                mProcs.find(vec[k]);
                            if (iter == mProcs.end()) continue;
                            param.vprocs.push_back(iter->second());
                        }
                    }
                }
            }
        }

        static void expand_value(
            std::string & value, 
            DataGraph & graph, 
            util::event::Event const & event)
        {
            std::string value2;
            std::string::size_type p1 = value.find('$');
            std::string::size_type p2 = 0;
            while (p1 != std::string::npos && p1 + 2 < value.size() && value[p1 + 1] == '{') {
                value2 += value.substr(p2, p1 - p2);
                ++p1;
                ++p1;
                p2 = value.rfind('}');
                if (p2 == std::string::npos) {
                    p2 = p1 - 2;
                    break;
                }
                std::string value3 = value.substr(p1, p2 - p1);
                expand_value(value3, graph, event);
                if (value3.compare(0, 7, "/event/") == 0) {
                    value2 += event.get_value(value3.substr(6));
                } else {
                    std::ostringstream oss;
                    graph.dump(value3, oss);
                    value2 += oss.str();
                }
                ++p2;
                p1 = value.find('$', p2);
            }
            value2 += value.substr(p2);
            value.swap(value2);
        }

        void ReportManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event, 
            ReportInfo const & info)
        {
            ReportInfo info2(info);
            DataGraph & graph(DataGraph::get(io_svc()));
            for (size_t i = 0; i < info.conds.size(); ++i) {
                std::string cond(info.conds[i]);
                expand_value(cond, graph, event);
                std::vector<std::string> args;
                framework::string::slice<std::string>(cond, std::back_inserter(args), " ");
                bool r = false;
                if (args.size() == 3) {
                    if (args[1] == "==" )
                        r = args[0] == args[2];
                    else if (args[1] == "!=" )
                        r = args[0] != args[2];
                    else if (args[1] == "=~" )
                        r = boost::regex_match(args[0], boost::regex(args[2]));
                }
                if (!r)
                    return;
            }
            for (size_t i = 0; i < info2.params.size(); ++i) {
                ReportParam & param(info2.params[i]);
                expand_value(param.value, graph, event);
                for (size_t j = 0; j < param.vprocs.size(); ++j) {
                    param.vprocs[j]->calc(param.value);
                }
            }
            submit(info2);
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
            boost::system::error_code const & ec)
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
            Url url(set_.base_url + info.url);
            for (size_t i = 0; i < info.params.size(); ++i) {
                url.param(info.params[i].name, info.params[i].value);
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
            LOG_DEBUG("[submit] " << info.name);
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
