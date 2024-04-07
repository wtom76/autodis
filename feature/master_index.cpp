#include "pch.hpp"
#include "master_index.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
feature::master_index::master_index(keeper::data_read& dr)
{
	dr.read_master_index(index_);
	assert(std::ranges::is_sorted(index_));

	map_.reserve(index_.size());
	index_pos_t i{0};
	for (index_value_t v : index_)
	{
		map_.emplace(v, i++);
	}
}
//---------------------------------------------------------------------------------------------------------
feature::master_index::index_pos_t feature::master_index::pos(index_value_t index_val) const
{
	auto const map_i{map_.find(index_val)};
	if (map_i == std::cend(map_))
	{
		throw master_missing_index_value{index_val};
	}
	return map_i->second;
}
//---------------------------------------------------------------------------------------------------------
feature::master_index::index_value_t feature::master_index::val(index_pos_t pos) const
{
	if (pos < 0 || static_cast<std::size_t>(pos) >= index_.size())
	{
		throw master_out_of_bounds{pos};
	}
	return index_[pos];
}
//---------------------------------------------------------------------------------------------------------
feature::master_index::index_value_t feature::master_index::next(index_value_t start, std::ptrdiff_t distance) const
{
	return val(pos(start) + distance);
}
