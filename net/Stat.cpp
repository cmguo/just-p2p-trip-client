// Stat.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Stat.h"

namespace trip
{
    namespace client
    {

        StatValue::StatValue()
            : total(0)
            , last(0)
            , speed(0)
            , speed_max(0)
            , speed_avg(0)
        {
        }

        void StatValue::push(
            boost::uint32_t n)
        {
            time = Time::now();
            total += n;
        }

        void StatValue::calc(
            Time const & now, 
            Time const & tlast)
        {
            speed = (total - last) * 1000 / (now - tlast).total_milliseconds();
            last = total;
            if (speed > speed_max)
                speed_max = speed;
            speed_avg = (speed_avg + speed) / 2;
        }

        Stat::Stat()
        {
            time_next_ += Duration::seconds(1);
        }

        void Stat::on_timer(
            Time const & now)
        {
            if (now >= time_next_) {
                recv_bytes_.calc(now, time_last_);
                send_bytes_.calc(now, time_last_);
                time_last_ = now;
                time_next_ += Duration::seconds(1);
            }
        }

        void Stat::push_recv(
            boost::uint32_t n)
        {
            recv_bytes_.push(n);
        }

        void Stat::push_send(
            boost::uint32_t n)
        {
            send_bytes_.push(n);
        }

    } // namespace client
} // namespace trip
