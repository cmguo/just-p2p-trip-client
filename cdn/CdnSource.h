// CdnSource.h

#ifndef _TRIP_CLIENT_CDN_CDN_SOURCE_H_
#define _TRIP_CLIENT_CDN_CDN_SOURCE_H_

#include "trip/client/ssp/SspSession.h"
#include "trip/client/core/Source.h"

#include <util/protocol/http/HttpClient.h>

namespace trip
{
    namespace client
    {

        class CdnManager;
        class SspTunnel;
        class M3u8Media;

        class CdnSource
            : public Source
            , public SspSession
        {
        public:
            CdnSource(
                CdnManager & manager, 
                SspTunnel & tunnel, 
                Resource & resource, 
                Url const & url);

            virtual ~CdnSource();

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
            
            virtual void cancel();

        protected:
            virtual void segment_url(
                Url & url, 
                size_t seg);

        private:
            void handle_m3u8_open(
                boost::system::error_code ec);

            void handle_open(
                boost::system::error_code ec);

            void handle_read(
                boost::system::error_code ec, 
                size_t bytes_read, 
                Piece::pointer piece);

            // make http range from ranges_, to head.range
            void make_range(
                util::protocol::HttpHead & head);

        private:
            virtual void on_timer(
                Time const & now);

            template <typename Archive>
            friend void serialize(
                Archive & ar, 
                CdnSource & t);

        private:
            CdnManager & manager_;
            util::protocol::HttpClient http_;
            M3u8Media * m3u8_;
            bool m3u8_is_open_;

        private:
            Piece::pointer piece_;
            struct PieceRange
            {
                DataId b;
                DataId e;
            };
            std::deque<PieceRange> ranges_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CDN_CDN_SOURCE_H_
