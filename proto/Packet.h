// Packet.h

#ifndef _TRIP_CLIENT_NET_PACKET_H_
#define _TRIP_CLIENT_NET_PACKET_H_

#include "trip/client/core/Piece.h"
#include "trip/client/proto/PacketBuffers.h"

#include <boost/iterator/iterator_facade.hpp>

namespace trip
{
    namespace client
    {

        class Packet
            : public PacketBuffers
        {
        public:
            typedef PacketBufferIterator const_iterator;

        public:
            Packet();

            ~Packet();

        public:
            const_iterator begin() const
            {
                return const_iterator(const_cast<Packet *>(this));
            }

            const_iterator end() const
            {
                return const_iterator();
            }

        public:
            void size(
                boost::uint16_t n)
            {
                size_ = n;
            }

            void prio(
                boost::uint16_t n)
            {
                prio_ = n;
            }

            bool swap_body(
                Piece::pointer & piece, 
                bool read);

        public:
            boost::uint16_t size() const
            {
                return size_;
            }

            boost::uint16_t prio() const
            {
                return prio_;
            }

            static size_t const HEAD_SIZE = 24;
            static size_t const TAIL_SIZE = 48;
            static size_t const MAX_SIZE = HEAD_SIZE + PIECE_SIZE + TAIL_SIZE;

        protected:
            friend class PacketBufferIterator;

            boost::uint16_t size_;
            boost::uint16_t prio_;      // Priority
            /* for udp, mtu = 1472 */
            boost::uint8_t head_[24];   // TunnelHeader:8, MessageHeader: 8, piece index: 8
            Piece::pointer body_;           // Piece size: 1400
            boost::uint8_t tail_[48];
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_UDP_PACKET_H_
