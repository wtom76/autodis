#pragma once

#include "framework.hpp"
#include <shared/data/frame.hpp>

namespace shared::data
{
	class frame;
}

namespace shared::math
{
	//---------------------------------------------------------------------------------------------------------
	// sma_delta
	// creates sma(period_short) - sma(period_long)
	//---------------------------------------------------------------------------------------------------------
	//[[nodiscard]]
	std::string sma_delta(data::frame& df, std::size_t src_idx, std::size_t period_short, std::size_t period_long);
}