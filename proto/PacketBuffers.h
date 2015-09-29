// PacketBuffers.h

#ifndef _TRIP_CLIENT_NET_PACKET_BUFFERS_H_
#define _TRIP_CLIENT_NET_PACKET_BUFFERS_H_

#include <util/buffers/SimpleBuffers.h>

#include <boost/iterator/iterator_facade.hpp>

namespace trip
{
    namespace client
    {

        class Packet;

        class PacketBufferIterator
            : public boost::iterator_facade<
                PacketBufferIterator,
                boost::asio::mutable_buffer,
                boost::forward_traversal_tag
              >
        {
        public:
            PacketBufferIterator(
                Packet * pkt = NULL)
                : pkt_(pkt)
            {
                if (pkt_)
                    increment();
            }

            Packet * packet()
            {
                return pkt_;
            }

        private:
            friend class boost::iterator_core_access;

            void increment();

            bool equal(
                PacketBufferIterator const & r) const
            {
                return boost::asio::buffer_cast<void *>(buf_) == 
                    boost::asio::buffer_cast<void *>(r.buf_);
            }

            reference dereference() const
            {
                return buf_;
            }

        private:
            Packet * pkt_;
            mutable boost::asio::mutable_buffer buf_;
        };

        typedef util::buffers::SimpleBuffers<
            PacketBufferIterator, 
            boost::uint8_t> PacketBuffers;

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_UDP_PACKET_BUFFERS_H_
