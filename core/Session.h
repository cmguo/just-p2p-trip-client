// Session.h

#ifndef _TRIP_CLIENT_CORE_SESSION_H_
#define _TRIP_CLIENT_CORE_SESSION_H_

#include "trip/client/core/Shaper.h"

namespace trip
{
    namespace client
    {

        class Session
            : public Shaper
        {
        public:
            Session(
                boost::asio::io_service & io_svc);

            virtual ~Session();

        public:
            boost::uint16_t id() const
            {
                return id_;
            }

            void id(
                boost::uint16_t n)
            {
                id_ = n;
            }

        private:
            boost::uint16_t id_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SESSION_H_
