#pragma once

#include "framework.hpp"
#include "data_uri.hpp"
#include "config.hpp"

namespace keeper
{
	//---------------------------------------------------------------------------------------------------------
	/// class data_read
	//---------------------------------------------------------------------------------------------------------
	class data_read
	{
	private:
		config const		cfg_;
		pqxx::connection	con_;

	public:
		data_read(config const& cfg);

		void read(std::vector<data_uri> const& src_uri, shared::data::frame& dest);
		void read(std::vector<long long> const& data_reg_ids, shared::data::frame& dest);
	};
}