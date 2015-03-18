// Finder.h

#ifndef _TRIP_CLIENT_CORE_FINDER_H_
#define _TRIP_CLIENT_CORE_FINDER_H_


namespace trip
{
    namespace client
    {

        class Scheduler;
        class Resource;
        class Source;

        class Finder
        {
        public:
            Finder();

            virtual ~Finder();

        public:
            void find(
                Scheduler & scheduler, 
                size_t count);

            void cancel(
                Scheduler & scheduler);

            void create_source(
                Scheduler & scheduler);

        protected:
            void found(
                Resource & resource, 
                std::vector<Url> & urls);

        private:
            virtual void find_more(
                Resource & resource, 
                size_t count) = 0;

            virtual Source * create(
                Scheduler & scheduler, 
                Url const & url) = 0;

        private:
            std::set<Scheduler *> requests_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_FINDER_H_

