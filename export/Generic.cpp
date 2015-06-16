// Generic.cpp

#include "trip/client/Common.h"
#include "trip/client/export/Export.h"
#include "trip/client/export/Generic.h"
#include "trip/client/export/Error.h"
#include "trip/client/main/Client.h"
#include "trip/client/Version.h"

#include <framework/logger/StreamRecord.h>
#include <framework/logger/Section.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Generic", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        class Generic
        {
        public:
            Generic(
                util::daemon::Daemon & daemon)
                : daemon_(daemon)
            {
            }

            PP_err start_engine()
            {
                LOG_SECTION();

                boost::system::error_code ec;
                if (daemon_.is_started()) {
                    ec = error::already_start;
                } else {
                    daemon_.start(1, ec);
                }

                return last_error(__FUNCTION__, ec);
            }

            PP_err stop_engine()
            {
                LOG_SECTION();

                boost::system::error_code ec;
                if (!daemon_.is_started()) {
                    ec = error::not_start;
                } else {
                    daemon_.stop(ec);
                }

                return last_error(__FUNCTION__, ec);
            }

            PP_ushort get_port(
                PP_str module)
            {
                boost::uint16_t port = 0;
                return port;
            }

            error::errors last_error(
                char const * log_title, 
                boost::system::error_code const & ec) const
            {
                if (ec && ec != boost::asio::error::would_block) {
                    LOG_WARN(log_title << ": " << ec.message());
                }
                trip::client::error::last_error(ec);
                return trip::client::error::last_error_enum(ec);
            }

            PP_err last_error() const
            {
                return trip::client::error::last_error_enum();
            }

            PP_str last_error_msg() const
            {
                static std::string err_msg;
                err_msg = trip::client::error::last_error().message();
                return err_msg.c_str();
            }

            PP_str version() const
            {
                return trip::client::version_string();
            }

            PP_err set_config(
                PP_str module, 
                PP_str section, 
                PP_str key, 
                PP_str value)
            {
                boost::system::error_code ec;
                if (NULL == section ||
                    NULL == key ||
                    NULL == value) {
                        ec = framework::system::logic_error::invalid_argument;
                } else {
                    ec = daemon_.config().set(section, key, value);
                }

                return last_error(__FUNCTION__, ec);
            }

        private:
            util::daemon::Daemon & daemon_;
        };

    } // namespace client
} // namespace trip

trip::client::Generic& the_generic()
{
    static trip::client::Generic generic(trip::client::global_daemon());
    return generic;
}

#if __cplusplus
extern "C" {
#endif // __cplusplus

    TRIP_DECL PP_err TRIP_StartEngine()
    {
        return the_generic().start_engine();
    }

    TRIP_DECL PP_ushort TRIP_GetPort(
        PP_str moduleName)
    {
        return the_generic().get_port(moduleName);
    }

    TRIP_DECL PP_err TRIP_StopEngine()
    {
        return the_generic().stop_engine();
    }

    TRIP_DECL PP_err TRIP_GetLastError()
    {
        return the_generic().last_error();
    }

    TRIP_DECL PP_str TRIP_GetLastErrorMsg()
    {
        return the_generic().last_error_msg();
    }

    TRIP_DECL PP_str TRIP_GetVersion()
    {
        return the_generic().version();
    }

    TRIP_DECL PP_err TRIP_SetConfig(
        PP_str module, 
        PP_str section, 
        PP_str key, 
        PP_str value)
    {
        return the_generic().set_config(module, section, key, value);
    }

#if __cplusplus
}
#endif // __cplusplus
