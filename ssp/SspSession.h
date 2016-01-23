// SspSession.h

#ifndef _TRIP_CLIENT_SSP_SSP_SESSION_H_
#define _TRIP_CLIENT_SSP_SSP_SESSION_H_

#include "trip/client/proto/Message.h"
#include "trip/client/net/Cell.h"

namespace trip
{
    namespace client
    {

        class SspTunnel;

        class SspSession
            : public Cell
        {
        public:
            SspSession();

            SspSession(
                SspTunnel & tunnel);

            virtual ~SspSession();

        public:
            SspTunnel & tunnel();

        public:
            virtual void on_send(
                //void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                //void * head, 
                NetBuffer & buf);

        protected:
            /*
            bool send_msg(
                Message * msg);

            virtual void on_msg(
                Message * msg);
            */
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_SSP_SSP_SESSION_H_
