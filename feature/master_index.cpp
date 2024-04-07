#include "pch.hpp"
#include "master_index.hpp"

//---------------------------------------------------------------------------------------------------------
feature::master_index::master_index(keeper::data_read& dr)
{
	dr.read_master_index(index_);
	assert(std::ranges::is_sorted(index_));

	map_.reserve(index_.size());
	std::ptrdiff_t i{0};
	for (index_value_t v : index_)
	{
		map_.emplace(v, i++);
	}
}
//---------------------------------------------------------------------------------------------------------
feature::master_index::index_value_t feature::master_index::next(index_value_t start, std::ptrdiff_t distance) const
{
	auto const map_i{map_.find(start)};
	if (map_i == std::cend(map_))
	{
		throw missing_index_value{};
	}
	std::ptrdiff_t const result_pos{map_i->second + distance};
	if (result_pos < 0 || static_cast<std::size_t>(result_pos) >= index_.size())
	{
		throw out_of_bounds{};
	}
	return index_[result_pos];
}
