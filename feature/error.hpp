#pragma once

#include "framework.hpp"

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class master_empty
		: public std::runtime_error
	{
	public:
		explicit master_empty()
			: std::runtime_error("master index is empty"s)
		{}
	};
	//---------------------------------------------------------------------------------------------------------
	// master index doesn't contain wanted value
	//---------------------------------------------------------------------------------------------------------
	class master_missing_index_value
		: public std::runtime_error
	{
	public:
		explicit master_missing_index_value(shared::data::frame::index_value_t idx)
			: std::runtime_error("master index doesn't contain wanted value "s + std::to_string(idx))
		{}
	};
	//---------------------------------------------------------------------------------------------------------
	// an attempt to access master index out of it's bounds
	//---------------------------------------------------------------------------------------------------------
	class master_out_of_bounds
		: public std::runtime_error
	{
	public:
		explicit master_out_of_bounds(std::ptrdiff_t pos)
			: std::runtime_error("an attempt to access master index data out of it's bounds "s + std::to_string(pos))
		{}
	};
	//---------------------------------------------------------------------------------------------------------
	// an attempt to access feature data out of it's index bounds
	//---------------------------------------------------------------------------------------------------------
	class feature_out_of_bounds
		: public std::runtime_error
	{
	public:
		explicit feature_out_of_bounds(shared::data::frame::index_value_t idx)
			: std::runtime_error("an attempt to access feature out of it's index bounds "s + std::to_string(idx))
		{}
	};
}