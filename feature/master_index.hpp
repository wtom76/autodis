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
		using index_t		= std::vector<index_value_t>;
		using map_t			= std::unordered_map<index_value_t, index_pos_t>; // index value -> position in index
	// data
	private:
		index_t index_;
		map_t	map_;
	// methods
	public:
		void load(keeper::data_read& dr);

		[[nodiscard]] std::size_t size() const noexcept { return index_.size(); }
		[[nodiscard]] index_pos_t pos(index_value_t index_val) const;
		[[nodiscard]] index_value_t at(index_pos_t pos) const;
		[[nodiscard]] index_value_t min() const;
		[[nodiscard]] index_value_t max() const;
		// not safe in regards to bounds
		[[nodiscard]] index_value_t next(index_value_t start, std::ptrdiff_t distance) const;
		// respecting bounds
		[[nodiscard]] index_value_t safe_next(index_value_t start, std::ptrdiff_t max_distance) const;
	};
}