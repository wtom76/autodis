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
	void track(data::frame const& src, std::size_t series_idx, size_t depth, data::frame& dest)
	{
		dest.clear();
		if (depth < 0)
		{
			return;
		}
		dest.append_series(src, {0});
		for (size_t i{1}; i <= depth; ++i)
		{
			dest.create_series("d(t-"s + std::to_string(i) + ")"s);
		}
		data::frame::series_t const& series_0{dest.series(0)};
		for (size_t d{1}; d <= depth; ++d)
		{
			data::frame::series_t const& series_d{dest.series(d)};
			for (size_t t{d}, t_d{0}; t < series_0.size(); ++t, ++t_d)
			{
				series_d[t] = series_0[t_d] - series_0[t];
			}
		}
	}
}