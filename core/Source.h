// Source.h

#ifndef _TRIP_CLIENT_CORE_SOURCE_H_
#define _TRIP_CLIENT_CORE_SOURCE_H_

#include "trip/client/core/Piece.h"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

namespace trip
{
    namespace client
    {

        class Scheduler;
        class Resource;

        class Source
        {
        public:
            Source(
                Resource & resource,
                Url const & url);

            virtual ~Source();

        public:
            void attach(
                Scheduler & scheduler);

            void detach();

            bool attached() const;

        public:
            Url const & url() const;

            bool has_segment(
                DataId sid) const;

            bool has_block(
                DataId bid) const;

        public:
            bool request(
                std::vector<DataId> const & pieces);


			boost::uint32_t get_windowsize() { return window_size_; }

			boost::uint32_t get_requests_count() 
			{
				return (boost::uint32_t)requesting_.size();
			}

			boost::uint32_t get_window_left() 
			{
				return get_windowsize() - get_requests_count();
			}

        protected:
            void on_ready();

            void on_map(
                DataId id, 
                boost::dynamic_bitset<> & map);

            void on_data(
                DataId id, 
                Piece::pointer piece);

	void check_timeout_tasks();
        private:
            virtual bool open(
                Url const & url) = 0;

            virtual bool close() = 0;

            virtual bool do_request(
                std::vector<DataId> const & pieces) = 0;

        protected:
            Resource & resource_;
            Url const url_;

        private:
            Scheduler * scheduler_;
            DataId map_id_;
            boost::dynamic_bitset<> map_;
            Duration delta_;
            Duration rtt_;
            struct Request
            {
                Request(DataId const & id, Time const & t)
                    : id(id), time(t) {}
                bool operator==(Request const & o) { return id == o.id; }
                DataId id;
                Time time;
            };
            std::deque<Request> requests_;
        };
			struct PieceTask 
			{
				boost::uint32_t start_ts_;
				boost::uint32_t timeout_ts_;
				DataId id_;
			};
			std::deque<PieceTask> requesting_;

			boost::uint32_t rtt_;
			boost::uint32_t delta_t_;
			boost::uint32_t window_size_;
		};

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SOURCE_H_