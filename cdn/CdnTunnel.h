// CdnTunnel.h

#ifndef _TRIP_CLIENT_CDN_CDN_TUNNEL_H_
#define _TRIP_CLIENT_CDN_CDN_TUNNEL_H_

namespace trip
{
    namespace client
    {

        class CdnSource;

        class CdnTunnel
        {
        public:
            CdnTunnel(
                boost::asio::io_service & io_svc);

            virtual ~CdnTunnel();

        public:
            void add(
                CdnSource * source);

            void del(
                CdnSource * source);

            boost::asio::io_service & io_svc()
            {
                return io_svc_;
            }

        public:
            virtual void on_timer(
                Time const & now);

        private:
            boost::asio::io_service & io_svc_;
            std::vector<CdnSource *> sources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CDN_CDN_TUNNEL_H_
