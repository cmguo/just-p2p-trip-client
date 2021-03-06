// P2pFinder.h

#ifndef _TRIP_CLIENT_P2P_P2P_FINDER_H_
#define _TRIP_CLIENT_P2P_P2P_FINDER_H_

#include "trip/client/proto/Message.h"
#include "trip/client/core/Finder.h"

#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class UdpManager;
        class P2pManager;
        class ResourceManager;
        struct MessageResponseFind;

        struct P2pEndpoint;

        class P2pFinder
            : public Finder
        {
        public:
            P2pFinder(
                P2pManager & manager);

            virtual ~P2pFinder();

        public:
            virtual std::string proto() const;

        public:
            virtual void find(
                Resource & resource, 
                size_t count);

        protected:
            virtual void init() = 0;

            virtual void send_msg(
                Message const & msg) = 0;

            bool handle_msg(
                Message const & msg);

            virtual boost::uint16_t get_id() = 0;

            virtual void set_id(
                boost::uint16_t id) = 0;

        public:
            void open();

            void close();

        private:
            void sync(
                int type, 
                Resource * res);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

            void handle_find(
                MessageResponseFind const & find);

        protected:
            template <typename Archive>
            friend void serialize(
                Archive & ar, 
                P2pFinder & t);

            P2pManager & pmgr_;
            UdpManager & umgr_;
            ResourceManager & rmgr_;
            bool inited_;
            bool opened_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_P2P_P2P_FINDER_H_
