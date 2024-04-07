#pragma once

#include "framework.hpp"
#include <keeper/keeper.hpp>

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// master_index
	//---------------------------------------------------------------------------------------------------------
	class master_index
	{
	// types
	private:
		using index_value_t = shared::data::frame::index_value_t;
		using index_t		= std::vector<index_value_t>;
		using map_t			= std::unordered_map<index_value_t, std::ptrdiff_t>; // index value -> position in index
	public:
		// index doens't contain wanted value
		class missing_index_value : public std::exception {};
		// an attempt to access index_ data out of it's bounds
		class out_of_bounds : public std::exception {};
	// data
	private:
		index_t index_;
		map_t	map_;
	// methods
	public:
		explicit master_index(keeper::data_read& dr);

		[[nodiscard]] index_value_t next(index_value_t start, std::ptrdiff_t distance) const;
	};
}