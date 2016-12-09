// Serialize.h

#ifndef _TRIP_CLIENT_DOWNLOAD_SERIALIZE_H_
#define _TRIP_CLIENT_DOWNLOAD_SERIALIZE_H_

#include "trip/client/download/DownloadManager.h"
#include "trip/client/download/Downloader.h"

#include <util/serialization/stl/map.h>
#include <util/serialization/Uuid.h>

namespace trip
{
    namespace client
    {

        /* Downloader */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Downloader & t)
        {
            ar & SERIALIZATION_NVP_3(t, sinks);
            ar & SERIALIZATION_NVP_3(t, master);
            ar & SERIALIZATION_NVP_3(t, download_point);
            ar & SERIALIZATION_NVP_3(t, play_point);
            ar & SERIALIZATION_NVP_3(t, sources);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            DownloadManager & t)
        {
            ar & SERIALIZATION_NVP_3(t, downloaders);
        }

    }
}

#endif // _TRIP_CLIENT_DOWNLOAD_SERIALIZE_H_
