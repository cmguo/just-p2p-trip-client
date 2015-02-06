// MessageHeader.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_HEADER_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_HEADER_H_

namespace trip
{
    namespace client
    {

        class MessageHeader
        {
        public:
            boost::uint16_t size;   // size
            boost::uint16_t sid;    // session id
            boost::uint16_t type;   // type
            boost::uint16_t pad;    // random pad data

        public:
            MessageHeader()
                : size(0)
                , sid(0)
                , type(0)
                , pad(rand())
            {
            }

        public:
            boost::uint16_t id() const
            {
                return type;
            }

            void id(
                boost::uint16_t n)
            {
                type = n;
            }

            boost::uint16_t data_size() const
            {
                return size;
            }

            void data_size(
                boost::uint16_t n)
            {
                size = n;
            }

        public:
            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & size
                    & sid
                    & type
                    & pad;
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_HEADER_H_
