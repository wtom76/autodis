#pragma once
#include "data_uri.hpp"

namespace keeper
{
	class config;

	//---------------------------------------------------------------------------------------------------------
	/// class data
	//---------------------------------------------------------------------------------------------------------
	class data
	{
	private:
		using buffer_t = std::vector<std::pair<long long, double>>;
		buffer_t data_;

	private:
		pqxx::connection con_;

	private:
		void _commit_buf();

	public:
		data(const config &cfg, data_uri const& dest_uri);

		void add(std::pair<long long, double> idx_val);
		void finish();
	};
}