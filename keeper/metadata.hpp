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
			long long	id_{0};
			std::string	data_uri_;
			std::string	feed_uri_;
			std::string	source_uri_;
		};

	private:
		pqxx::connection con_;

	public:
		metadata(const config& cfg);
		std::vector<series_info> load();
	};
}