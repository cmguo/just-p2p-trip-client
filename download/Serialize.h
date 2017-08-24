// Serialize.h

#ifndef _TRIP_CLIENT_DOWNLOAD_SERIALIZE_H_
#define _TRIP_CLIENT_DOWNLOAD_SERIALIZE_H_

#include "trip/client/download/DownloadManager.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/download/CdnDownloader.h"

#include <util/serialization/stl/map.h>
#include <util/serialization/stl/set.h>

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
            CdnDownloader::SegmentInfo & t)
        {
            ar & SERIALIZATION_NVP_1(t, meta_ready);
            ar & SERIALIZATION_NVP_1(t, pos);
            ar & SERIALIZATION_NVP_1(t, end);
            ar & SERIALIZATION_NVP_1(t, timeout_pieces);
            ar & SERIALIZATION_NVP_1(t, np2p);
            ar & SERIALIZATION_NVP_1(t, nsource);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            CdnDownloader & t)
        {
            serialize(ar, (Downloader &)t);
            ar & SERIALIZATION_NVP_3(t, win_beg);
            ar & SERIALIZATION_NVP_3(t, win_end);
            ar & SERIALIZATION_NVP_3(t, segments);
            ar & SERIALIZATION_NVP_3(t, timeout_seg);
            ar & SERIALIZATION_NVP_3(t, full_seg);
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
