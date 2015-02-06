// Error.cpp

#include "trip/client/Common.h"
#include "trip/client/export/Error.h"

#include <util/protocol/http/HttpError.h>

#include <boost/asio/error.hpp>

namespace trip
{
    namespace client
    {

        namespace error
        {

            error::errors last_error_enum(
                boost::system::error_code const & ec)
            {
                if (!ec) {
                    return success;
                } else if (ec == boost::asio::error::would_block) {
                    return would_block;
                } else if (ec == boost::asio::error::operation_aborted) {
                    return operation_canceled;
                } else if (ec == boost::asio::error::connection_aborted) {
                    return operation_canceled;
                } else if (ec.category() == trip::client::error::get_category()) {
                    return (error::errors)ec.value();
                } else if (ec.category() == framework::system::logic_error::get_category()) {
                    return logic_error;
                } else if (ec.category() == util::protocol::http_error::get_category()) {
                    return network_error;
                } else if (ec.category() == boost::asio::error::get_system_category()) {
                    return network_error;
                } else if (ec.category() == boost::asio::error::get_netdb_category()) {
                    return network_error;
                } else if (ec.category() == boost::asio::error::get_addrinfo_category()) {
                    return network_error;
                } else if (ec.category() == boost::asio::error::get_misc_category()) {
                    return network_error;
                } else {
                    return other_error;
                }
            }

        } // namespace error

    } // namespace client
} // namespace trip
