// WorkQueue.h

#ifndef _TRIP_CLIENT_CACHE_WORK_QUEUE_H_
#define _TRIP_CLIENT_CACHE_WORK_QUEUE_H_

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>

namespace trip
{
    namespace client
    {

        class WorkQueue
            : public boost::asio::io_service
        {
        public:
            WorkQueue(
                boost::asio::io_service & io_svc);

            ~WorkQueue();

        public:
            template <
                typename Work, 
                typename Callback
            >
            struct WorkCallback
            {
                WorkCallback(
                    boost::asio::io_service & io_svc,
                    Work const & work, 
                    Callback const & callback)
                    : io_svc_(io_svc)
                    , work_(work)
                    , callback_(callback)
                {
                }

                struct Call
                {
                    Call(
                        Callback const & callback, 
                        typename Work::result_type result)
                        : callback_(callback)
                        , result_(result)
                    {
                    }

                    void operator()() const
                    {
                        callback_(result_);
                    }

                private:
                    Callback callback_;
                    typename Work::result_type result_;
                };

                void operator()() const
                {
                    typename Work::result_type result = work_();
                    io_svc_.post(Call(callback_, result));
                }

            private:
                boost::asio::io_service & io_svc_;
                Work work_;
                Callback callback_;
            };

            template <
                typename Work, 
                typename Callback
            >
            void post(
                Work const & work, 
                Callback const & callback)
            {
                post(WorkCallback<Work, Callback>(io_svc_, work, callback));
            }

            bool stop();

        private:
            boost::asio::io_service & io_svc_;
            boost::asio::io_service::work work_;
            boost::thread thread_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_WORK_QUEUE_H_
