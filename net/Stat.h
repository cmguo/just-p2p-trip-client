// Stat.h

#ifndef _TRIP_CLIENT_NET_STAT_H_
#define _TRIP_CLIENT_NET_STAT_H_

namespace trip
{
    namespace client
    {

        struct StatValue
        {
            boost::uint64_t total;
            boost::uint64_t last;
            boost::uint64_t speed;
            boost::uint64_t speed_max;
            boost::uint64_t speed_avg;
            Time time;

            StatValue();

            void push(
                boost::uint32_t n);

            void calc(
                Time const & now, 
                Time const & last);
        };

        class Stat
        {
        public:
            Stat();

        public:
            void push_recv(
                boost::uint32_t n);

            void push_send(
                boost::uint32_t n);

            void on_timer(
                Time const & now);
            
        public:
            StatValue const & recv_bytes() const { return recv_bytes_; }

            StatValue const & send_bytes() const { return send_bytes_; }

        private:
            Time time_next_;
            Time time_last_;
            StatValue recv_bytes_;            
            StatValue send_bytes_;            
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_NET_STAT_H_

