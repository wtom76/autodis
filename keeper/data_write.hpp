#pragma once
#include "framework.hpp"
#include "data_uri.hpp"

namespace keeper
{
	class config;

	//---------------------------------------------------------------------------------------------------------
	/// class data_write
	//---------------------------------------------------------------------------------------------------------
	class data_write
	{
	private:
		using record_t = std::pair<long long, std::vector<double>>;
		using buffer_t = std::vector<record_t>;
		buffer_t data_;

	private:
		pqxx::connection		con_;
		std::vector<data_uri>	dest_uri_;

	private:
		void _prepare_queries();
		void _commit_buf();

	public:
		data_write(config const& cfg, std::vector<data_uri> dest_uri);

		void add(std::pair<long long, std::vector<double>>&& idx_val);
		void finish();
	};
}