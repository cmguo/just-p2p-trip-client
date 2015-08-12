// CdnTunnel.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnTunnel.h"
#include "trip/client/cdn/CdnSource.h"

namespace trip
{
    namespace client
    {

        CdnTunnel::CdnTunnel(
            boost::asio::io_service & io_svc)
            : io_svc_(io_svc)
        {
        }

        CdnTunnel::~CdnTunnel()
        {
        }

        void CdnTunnel::add(
            CdnSource * source)
        {
            sources_.push_back(source);
        }

        void CdnTunnel::del(
            CdnSource * source)
        {
            sources_.erase(std::remove(
                    sources_.begin(), 
                    sources_.end(), source), 
                sources_.end());
        }

        void CdnTunnel::on_timer(
            Time const & now)
        {
            for (size_t i = 0; i < sources_.size(); ++i)
                sources_[i]->on_timer(now);
        }

    } // namespace client
} // namespace trip
