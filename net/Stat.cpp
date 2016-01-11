// Stat.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Stat.h"

namespace trip
{
    namespace client
    {

        Stat::Stat()
            : bytes_(0)
            , last_bytes_(0)
        {
            next_time_ += Duration::seconds(1);
        }

        void Stat::on_timer(
            Time const & now)
        {
        }

        void Stat::push(
            size_t n)
        {
            time_ = Time::now();
            bytes_ += n;
            if (time_ >= next_time_) {
                boost::uint64_t speed = (bytes_ - last_bytes_) * 1000 / (time_ - last_time_).total_milliseconds();
                next_time_ += Duration::seconds(1);
                if (speed > max_speed_)
                    max_speed_ = speed;
                avg_speed_ = (avg_speed_ + speed) / 2;
            }
        }

    } // namespace client
} // namespace trip
