
#ifndef _TRIP_CLIENT_CORE_FINDER_H_
#define _TRIP_CLIENT_CORE_FINDER_H_

#include <boost/function.hpp>

namespace trip
{
    namespace client
    {

        class Resource;
        class Source;
        class Scheduler;

        class Finder
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &, 
                    std::vector<Url> &)
            > resp_t;

        public:
            Finder();

            virtual ~Finder();

        public:
            virtual std::string proto() const = 0;

            virtual void find(
                Resource & resource, 
                size_t count, 
                resp_t const & resp) = 0;

            virtual void cancel(
                Resource & resource) = 0;

            virtual Source * create(
                Scheduler & scheduler, 
                Url const & url) = 0;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_FINDER_H_

