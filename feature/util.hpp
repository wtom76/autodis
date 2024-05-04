#pragma once

#include "framework.hpp"
#include "master_index.hpp"
#include "abstract.hpp"

namespace feature
{
	// find master index value max_distance or less from start not counting nans in feat
	// actual distance by module can be less than max_distance if it exceeds feat bounds
	// returns index value and actual distance
	std::pair<master_index::index_value_t, std::ptrdiff_t> solid_next(master_index const& mi, abstract& feat, master_index::index_value_t start, std::ptrdiff_t max_distance);
}