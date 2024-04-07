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
	public:
		using index_value_t = shared::data::frame::index_value_t;
		using index_pos_t	= ptrdiff_t;
	private:
		using index_t		= std::vector<index_value_t>;
		using map_t			= std::unordered_map<index_value_t, index_pos_t>; // index value -> position in index
	// data
	private:
		index_t index_;
		map_t	map_;
	// methods
	public:
		explicit master_index(keeper::data_read& dr);

		[[nodiscard]] index_pos_t pos(index_value_t index_val) const;
		[[nodiscard]] index_value_t val(index_pos_t pos) const;
		[[nodiscard]] index_value_t next(index_value_t start, std::ptrdiff_t distance) const;
	};
}