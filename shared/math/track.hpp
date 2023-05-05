#pragma once

#include "framework.hpp"
#include "../data/frame.hpp"

namespace shared::math
{
	//---------------------------------------------------------------------------------------------------------
	// track
	// creates x(t-1) - x(t), ..., x(t-n) - x(t)
	// n = depth
	//---------------------------------------------------------------------------------------------------------
	void track(data::frame& df, std::size_t src_idx, std::size_t depth)
	{
		if (!depth)
		{
			return;
		}
		std::string src_name{df.name(src_idx)};
		std::size_t first_dst_idx{df.col_count()};
		for (std::size_t i{1}; i <= depth; ++i)
		{
			df.create_series(src_name + "_delta(t-"s + std::to_string(i) + ")"s);
		}
		data::frame::series_t const& src_series{df.series(src_idx)};
		for (std::size_t dst_idx{0}; dst_idx < depth; ++dst_idx)
		{
			data::frame::series_t& dst_series{df.series(first_dst_idx + dst_idx)};
			for (std::size_t t{dst_idx + 1}, t_d{0}; t < src_series.size(); ++t, ++t_d)
			{
				dst_series[t] = src_series[t_d] - src_series[t];
			}
		}
	}
}