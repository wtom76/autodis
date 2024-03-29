#pragma once

#include "framework.hpp"

namespace shared::data
{
	class frame;
}

namespace shared::math
{
	//---------------------------------------------------------------------------------------------------------
	// track
	// creates x(t-1) - x(t), ..., x(t-n) - x(t)
	// n = depth
	//---------------------------------------------------------------------------------------------------------
	//[[nodiscard]]
	std::vector<std::string> track(data::frame& df, std::size_t src_idx, std::size_t depth);
}