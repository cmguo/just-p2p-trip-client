// Finder.h

#ifndef _TRIP_CLIENT_CORE_FINDER_H_
#define _TRIP_CLIENT_CORE_FINDER_H_


namespace trip
{
    namespace client
    {

        class Resource;

        class Finder
        {
        public:
            virtual ~Finder() {}

        public:
            virtual void find_more(
                Resource & resource) = 0;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_FINDER_H_

