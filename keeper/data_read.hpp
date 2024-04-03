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
		using value_t = shared::data::frame::value_t;
		struct bounds
		{
			index_value_t index_min_;
			index_value_t index_max_;
			value_t value_min_;
			value_t value_max_;
		};
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
		bounds read_bounds(long long data_reg_id);
	};
}