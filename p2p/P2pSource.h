// P2pSource.h

#ifndef _TRIP_CLIENT_P2P_P2P_SOURCE_H_
#define _TRIP_CLIENT_P2P_P2P_SOURCE_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/core/Source.h"

namespace trip
{
    namespace client
    {

        class Resource;
        class UdpTunnel;

        class P2pSource
            : public Source
            , UdpSession
        {
        public:
            P2pSource(
                Resource & resource,
                UdpTunnel & tunnel, 
                Url const & url);

            virtual ~P2pSource();

        public:
            virtual bool has_segment(
                DataId sid) const;

            virtual bool has_block(
                DataId bid) const;

            virtual boost::uint32_t window_size() const;

            virtual boost::uint32_t window_left() const;

        public:
            virtual bool open(
                Url const & url);
            
            virtual bool close();
            
            virtual bool request(
                std::vector<DataId> const & pieces);
            
            virtual void seek(
                DataId id);

        private:
            void req_map(
                DataId id);

            static UdpTunnel & get_tunnel(
                boost::asio::io_service & io_svc,
                Url const & url);

        private:
            virtual void on_msg(
                Message * msg);

            virtual void on_timer(
                Time const & now);

        private:
            void on_map(
                DataId id,
                boost::dynamic_bitset<boost::uint32_t> & map);

            void on_data(
                DataId id, 
                Piece::pointer piece);

        private:
            struct Request
            {
                Request(DataId const & id, Time const & t)
                    : id(id), time(t) {}
                bool operator==(Request const & o) { return id == o.id; }
                DataId id;
                Time time;
            };

        private:
            DataId map_id_;
            boost::dynamic_bitset<boost::uint32_t> map_;
            Duration delta_;
            Duration rtt_;
            Request map_req_; // timer for map request
            std::deque<Request> requests_;
            boost::uint32_t req_count_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_SOURCE_H_
