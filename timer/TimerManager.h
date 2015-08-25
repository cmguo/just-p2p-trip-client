// TimerManager.h

#ifndef _TRIP_CLIENT_TIMER_TIMER_MANAGER_H_
#define _TRIP_CLIENT_TIMER_TIMER_MANAGER_H_

#include "trip/client/timer/TimeEvent.h"

#include <util/daemon/Module.h>
#include <util/event/Observable.h>

#include <framework/timer/TimeTraits.h>

#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace trip
{
    namespace client
    {

        class TimerManager
            : public util::daemon::ModuleBase<TimerManager>
            , public util::event::Observable
        {
        public:
            TimerManager(
                util::daemon::Daemon & daemon);

            ~TimerManager();

        public:
            TimeEvent t_10_ms;

            TimeEvent t_100_ms;

            TimeEvent t_1_s;

            TimeEvent t_10_s;

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void handle_timer(
                boost::system::error_code const & ec, 
                boost::weak_ptr<void> const & cancel_token);

            void handle_tick();

        private:
            boost::asio::basic_deadline_timer<framework::timer::ClockTime> timer_;
            boost::shared_ptr<void> cancel_token_;
            Time expire_10ms_;
            Time expire_100ms_;
            Time expire_1s_;
            Time expire_10s_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_TIMER_TIMER_MANAGER_H_
