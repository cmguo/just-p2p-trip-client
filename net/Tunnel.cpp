// Tunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/net/Tunnel.h"
#include "trip/client/net/Fifo.h"

namespace trip
{
    namespace client
    {

        Tunnel::Tunnel(
            Bus * bus)
            : Bus(bus, new Fifo)
        {
        }

        Tunnel::~Tunnel()
        {
        }

        void Tunnel::on_send(
            //void * head, 
            NetBuffer & buf)
        {
            Bus::on_send(/*head, */buf);
        }

        void Tunnel::on_recv(
            //void * head, 
            NetBuffer & buf)
        {
            Bus::on_recv(/*head, */buf);
        }

    } // namespace client
} // namespace trip
