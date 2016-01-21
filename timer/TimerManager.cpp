// TimerManager.cpp

#include "trip/client/Common.h"
#include "trip/client/timer/TimerManager.h"
#include "trip/client/utils/Serialize.h"

#include <util/event/EventEx.hpp>

#include <framework/logger/Logger.h>
#include <framework/logger/Section.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            TimeEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, now);
        }

        TimerManager::TimerManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<TimerManager>(daemon, "trip.client.TimerManager")
            , util::event::Observable("trip.client.TimerManager") 
            , t_10_ms("t_10_ms")
            , t_100_ms("t_100_ms")
            , t_1_s("t_1_s")
            , t_10_s("t_10_s")
            , timer_(io_svc())
        {
            register_event(t_10_ms);
            register_event(t_100_ms);
            register_event(t_1_s);
            register_event(t_10_s);
        }

        TimerManager::~TimerManager()
        {
        }

        struct noop_deleter { void operator()(void*) {} };

        bool TimerManager::startup(
            boost::system::error_code & ec)
        {
            expire_10ms_ += Duration::milliseconds(10);
            expire_100ms_ += Duration::milliseconds(100);
            expire_1s_ += Duration::seconds(1);
            expire_10s_ += Duration::seconds(10);
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
            Time now;
            do { expire_10ms_ += Duration::milliseconds(10); }
            while (now >= expire_10ms_);
            t_10_ms.now = now;
            if (now >= expire_100ms_) {
                do { expire_100ms_ += Duration::milliseconds(100); }
                while (now >= expire_100ms_);
                t_100_ms.now = now;
                if (now >= expire_1s_) {
                    do { expire_1s_ += Duration::seconds(1); }
                    while (now >= expire_1s_);
                    t_1_s.now = now;
                    if (now >= expire_10s_) {
                        do { expire_10s_ += Duration::seconds(10); }
                        while (now >= expire_10s_);
                        t_10_s.now = now;
                        raise(t_10_s);
                        //get_daemon().dump();
                    }
                    raise(t_1_s);
                }
                raise(t_100_ms);
            }
            raise(t_10_ms);
        }

    } // namespace client
} // namespace trip
