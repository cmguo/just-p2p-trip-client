// WorkQueue.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/WorkQueue.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        WorkQueue::WorkQueue(
            boost::asio::io_service & io_svc)
            : io_svc_(io_svc)
            , work_(*this)
            , thread_(boost::bind(&WorkQueue::run, this))
        {
        }

        WorkQueue::~WorkQueue()
        {
            stop();
        }

        bool WorkQueue::stop()
        {
            boost::asio::io_service::stop();
            thread_.join();
            return true;
        }

    } // namespace client
} // namespace trip
