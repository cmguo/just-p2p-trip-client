#ifndef _TRIP_CORE_SOURCE_H_
#define _TRIP_CORE_SOURCE_H_

namespace trip
{
    namspace core
    {

        class Source
        {
        public:
            Source(
                boost::asio::io_service & io_svc);

            virtual ~Source();

        public:
            virtual bool open(
                framework::string::Url const & url, 
                :

        };

    }
}

#endif // _TRIP_CORE_SOURCE_H_
