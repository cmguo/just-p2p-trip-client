// UrlHelper.h

#ifndef _TRIP_CLIENT_URL_HELPER_H_
#define _TRIP_CLIENT_URL_HELPER_H_

#include <framework/string/Url.h>
#include <framework/configure/Config.h>

namespace trip
{
    namespace client
    {

            bool decode_url(
                framework::string::Url & url, 
                boost::system::error_code & ec);

            void apply_config(
                framework::configure::Config & config, 
                framework::string::Url const & url, 
                std::string const & prefix);

            bool decode_param(
                framework::string::Url & url, 
                std::string const & key, 
                boost::system::error_code & ec);

            void apply_params(
                framework::string::Url & dst, 
                framework::string::Url const & src);

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_URL_HELPER_H_
