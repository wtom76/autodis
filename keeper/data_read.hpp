#pragma once

#include "framework.hpp"
#include "data_uri.hpp"
#include "config.hpp"
#include "metadata.hpp"

namespace keeper
{
	//---------------------------------------------------------------------------------------------------------
	/// class data_read
	//---------------------------------------------------------------------------------------------------------
	class data_read
	{
	// types
	public:
		using index_value_t = shared::data::frame::index_value_t;
		using index_bounds_t = std::pair<index_value_t, index_value_t>;
	// data
	private:
		config const		cfg_;
		pqxx::connection	con_;

	// methods
	private:
		void _read(metadata::data_info const& mf, shared::data::frame& dst);
	public:
		data_read(config const& cfg);

		void read(std::vector<data_uri> const& src_uri, shared::data::frame& dest);
		void read(std::vector<long long> const& data_reg_ids, shared::data::frame& dest);
		index_bounds_t read_index_bounds(long long data_reg_id);
	};
}