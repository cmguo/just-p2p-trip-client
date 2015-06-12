// TimerManager.cpp

#include "trip/client/Common.h"
#include "trip/client/timer/TimerManager.h"

#include "framework/logger/Logger.h"
#include "framework/logger/Section.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        TimerManager::TimerManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<TimerManager>(daemon, "TimerManager")
            , timer_(io_svc())
        {
        }

        TimerManager::~TimerManager()
        {
        }

        struct noop_deleter { void operator()(void*) {} };

        bool TimerManager::startup(
            boost::system::error_code & ec)
        {
            expire_10ms_ += framework::timer::Duration::milliseconds(10);
            expire_100ms_ += framework::timer::Duration::milliseconds(100);
            cancel_token_.reset(static_cast<void*>(0), noop_deleter());
            timer_.expires_at(expire_10ms_);
            timer_.async_wait(boost::bind(&TimerManager::handle_timer, 
                this, 
                _1, 
                boost::weak_ptr<void>(cancel_token_)));
            return true;
        }

        bool TimerManager::shutdown(
            boost::system::error_code & ec)
        {
            cancel_token_.reset(static_cast<void*>(0), noop_deleter());
            timer_.cancel(ec);
            return true;
        }

        void TimerManager::handle_timer(
            boost::system::error_code const & ec, 
            boost::weak_ptr<void> const & cancel_token)
        {
            LOG_SECTION()

            if (!cancel_token.expired()) {
                handle_tick();
                timer_.expires_at(expire_10ms_);
                timer_.async_wait(boost::bind(&TimerManager::handle_timer, 
                    this, 
                    _1, 
                    boost::weak_ptr<void>(cancel_token)));
            }
        }

        void TimerManager::handle_tick()
        {
            expire_10ms_ += framework::timer::Duration::milliseconds(10);
            t_10_ms.now = framework::timer::Time::now();
            if (t_10_ms.now >= expire_100ms_) {
                expire_100ms_ += framework::timer::Duration::milliseconds(100);
                t_100_ms.now = t_10_ms.now;
                if (t_100_ms.now >= expire_1s_) {
                    expire_1s_ += framework::timer::Duration::seconds(1);
                    t_1_s.now = t_10_ms.now;
                    raise(t_1_s);
                }
                raise(t_100_ms);
            }
            raise(t_10_ms);
        }

    } // namespace client
} // namespace trip
