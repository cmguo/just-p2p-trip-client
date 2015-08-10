// CdnSource.h

#ifndef _TRIP_CLIENT_CDN_CDN_SOURCE_H_
#define _TRIP_CLIENT_CDN_CDN_SOURCE_H_

#include "trip/client/core/Source.h"

#include <util/protocol/http/HttpClient.h>

namespace trip
{
    namespace client
    {

        class CdnTunnel;

        class CdnSource
            : public Source
        {
        public:
            CdnSource(
                CdnTunnel & tunnel, 
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
            
        private:
            void handle_open(
                boost::system::error_code ec);

            void handle_read(
                boost::system::error_code ec, 
                size_t bytes_read, 
                Piece::pointer piece);

        private:
            friend class CdnTunnel;
            void on_timer();

        private:
            util::protocol::HttpClient http_;
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
