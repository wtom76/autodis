#pragma once
#include "framework.hpp"
#include <pqxx/pqxx>

namespace keeper
{
	class config;

	//---------------------------------------------------------------------------------------------------------
	/// class metadata
	//---------------------------------------------------------------------------------------------------------
	class metadata
	{
	public:
		struct series_info
		{
			using id_t = long long;
			id_t		id_{0};
			std::string	data_uri_;
			std::string	feed_uri_;
			std::string	source_uri_;
			bool		pending_{false};	// flag to update data from source
		};

	private:
		pqxx::connection con_;

	public:
		metadata(const config& cfg);
		std::vector<series_info> load();
		void drop_pending_flag(series_info::id_t series_id);
	};
}