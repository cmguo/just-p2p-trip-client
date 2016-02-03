
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
                void (
                    std::vector<Source *> const &)
            > resp_t;

        public:
            Finder();

            virtual ~Finder();

        public:
            virtual std::string proto() const = 0;

            virtual void find(
                Resource & resource, 
                size_t count, 
                resp_t const & resp);

            virtual void cancel(
                Resource & resource);
            
        protected:
            void found(
                Uuid const & rid, 
                std::vector<Source *> const & sources);

        private:
            virtual void find(
                Resource & resource, 
                size_t count) = 0;

        private:
            std::map<Uuid, std::pair<size_t, resp_t> > requests_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_FINDER_H_

