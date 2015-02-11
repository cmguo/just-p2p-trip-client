// HttpSession.cpp

#include "trip/client/Common.h"
#include "trip/client/http/HttpSession.h"
#include "trip/client/utils/UrlHelper.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.HttpSession", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        HttpSession::HttpSession(
            boost::asio::io_service & io_svc, 
            Resource & resource)
            : Sink(resource)
            , io_svc_(io_svc)
        {
        }

        HttpSession::~HttpSession()
        {
        }

        bool HttpSession::empty() const
        {
            return requests_.empty();
        }

        void HttpSession::async_prepare(
            HttpServer * request, 
            response_t const & resp)
        {
            requests_[request] = resp;
        }

        bool HttpSession::close_request(
            HttpServer * request, 
            boost::system::error_code & ec)
        {
            requests_.erase(request);
            return true;
        }

    } // namespace dispatch
} // namespace trip
