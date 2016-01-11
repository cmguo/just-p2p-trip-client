// Stat.h

#ifndef _TRIP_CLIENT_NET_STAT_H_
#define _TRIP_CLIENT_NET_STAT_H_

namespace trip
{
    namespace client
    {

        class Stat
        {
        public:
            Stat();

        public:
            void push(
                size_t n);

            void on_timer(
                Time const & now);
            
        public:
            Time time() const { return time_; }

            boost::uint64_t bytes() const { return bytes_; }

            boost::uint64_t avg_speed() const { return avg_speed_; }

            boost::uint64_t max_speed() const { return max_speed_; }

        private:
            Time time_;
            Time last_time_;
            Time next_time_;
            boost::uint64_t bytes_;            
            boost::uint64_t last_bytes_;            
            boost::uint64_t last_speed_;
            boost::uint64_t max_speed_;
            boost::uint64_t avg_speed_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_STAT_H_

