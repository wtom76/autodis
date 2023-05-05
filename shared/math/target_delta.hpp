#pragma once

#include "framework.hpp"
#include "../data/frame.hpp"

namespace shared::math
{
	//---------------------------------------------------------------------------------------------------------
	// target_delta
	// creates x(t+1) - x(t)
	//---------------------------------------------------------------------------------------------------------
	void target_delta(data::frame const& src, std::size_t series_idx, data::frame& dst)
	{
		auto dst_i{dst.create_series(src.names()[series_idx] + "_delta(t+1)"s)->begin()};
		auto const src_e{src.series(series_idx).cend()};
		auto src_t_1_i{src.series(series_idx).cbegin()};
		if (src_t_1_i == src_e)
		{
			return;
		}
		auto src_t_i{src_t_1_i++};
		for (; src_t_1_i != src_e; ++src_t_1_i, ++src_t_i, ++dst_i)
		{
			*dst_i = *src_t_1_i - *src_t_i;
		}
	}
}