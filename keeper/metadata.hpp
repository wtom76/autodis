#pragma once
#include <pqxx/pqxx>
#include "framework.hpp"
#include "data_uri.hpp"

namespace keeper
{
	class config;

	//---------------------------------------------------------------------------------------------------------
	/// class metadata
	//---------------------------------------------------------------------------------------------------------
	class metadata
	{
	public:
		//---------------------------------------------------------------------------------------------------------
		// struct feed_info
		//---------------------------------------------------------------------------------------------------------
		struct feed_info
		{
			std::string					feed_uri_;
			std::vector<std::string>	feed_args_;
			std::vector<data_uri>		data_uri_;
		};
		//---------------------------------------------------------------------------------------------------------
		// struct source_info
		//---------------------------------------------------------------------------------------------------------
		struct source_info
		{
			long long	source_id_{0};
			std::string	source_uri_;
			std::string	source_args_;
			bool		pending_{false};	// flag that source has new data to fetch
			bool		always_{false};		// true if pending_ shouldn't be dropped after data acquisition
			feed_info	dest_;
		};
		//---------------------------------------------------------------------------------------------------------
		// struct data_info
		//---------------------------------------------------------------------------------------------------------
		struct data_info
		{
			long long	data_id_{0};
			data_uri	data_uri_;
			std::string	description_;
		};

	private:
		pqxx::connection con_;

	private:
		std::vector<data_info> _load_data_info();
	public:
		metadata(const config& cfg);
		std::vector<source_info> load();
		void load_data_info(std::vector<long long> const& reg_data_ids, std::vector<data_info>& dest);
		void drop_pending_flag(long long source_id);
	};
}