// ReportManager.h

#ifndef _JUST_MAIN_REPORT_MANAGER_H_
#define _JUST_MAIN_REPORT_MANAGER_H_

#include "trip/client/main/ReportInfo.h"

#include <util/daemon/Module.h>

#include <framework/timer/TimeTraits.h>

#include <boost/asio/io_service.hpp>

namespace util
{
    namespace protocol
    {
        class HttpClient;
    }
}

namespace trip
{
    namespace client
    {

        class ReportManager
            : public util::daemon::ModuleBase<ReportManager>
        {
        public:
            ReportManager(
                util::daemon::Daemon & daemon);

            ~ReportManager();

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        public:
            void report(
                 std::string const & info);

        private:
            void on_boot_ready();

        private:
            void submit(
                ReportInfo const & info);

            void submit2(
                ReportInfo const & info);

            std::string encrypt(
                std::string const & str);

            std::string decrypt(
                std::string const & str);

            void handle_fetch_all(
                boost::system::error_code const & ec);

            void handle_fetch(
                boost::system::error_code const & ec);

        private:
            util::protocol::HttpClient * http_client_;
            ReportSet set_;
            std::deque<Url> tasks_;
        };

    } // namespace client
} // namespace trip

#endif // _JUST_MAIN_REPORT_MANAGER_H_
