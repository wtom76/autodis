#include "pch.hpp"
#include "sma_delta.hpp"
#include "../data/frame.hpp"

namespace shared::math
{
	//---------------------------------------------------------------------------------------------------------
	// TODO: apply std::simd?
	std::string sma_delta(data::frame& df, std::size_t src_idx, std::size_t period_short, std::size_t period_long)
	{
		assert(period_short > 0);
		assert(period_long > period_short);

		data::frame::series_t const& src_series{df.series(src_idx)};
		std::string const generated_name{df.name(src_idx) + "_sma_delta("s + std::to_string(period_short) + '-' + std::to_string(period_long) + ')'};
		data::frame::series_t& dst_series{*df.create_series(generated_name, shared::data::nan)};

		data::frame::value_t sma_short{};
		data::frame::value_t sma_long{};
		std::size_t i{0};
		std::deque<data::frame::value_t> values_short;
		std::deque<data::frame::value_t> values_long;
		for (; i != df.row_count() && values_long.size() != period_long - period_short; ++i)
		{
			if (std::isnormal(src_series[i]))
			{
				sma_long += src_series[i];
				values_long.push_back(src_series[i]);
			}
		}
		for (; i != df.row_count() && values_long.size() != period_long; ++i)
		{
			if (std::isnormal(src_series[i]))
			{
				sma_short += src_series[i];
				sma_long += src_series[i];
				values_long.push_back(src_series[i]);
				values_short.push_back(src_series[i]);
			}
		}
		if (values_long.size() < period_long)
		{
			return generated_name;
		}
		assert(values_short.size() == period_short);
		sma_short /= period_short;
		sma_long /= period_long;
		dst_series[i - 1] = sma_short - sma_long;
		for (; i != df.row_count(); ++i)
		{
			if (std::isnormal(src_series[i]))
			{
				sma_short += (src_series[i] - values_short.front()) / period_short;
				sma_long += (src_series[i] - values_long.front()) / period_long;
				values_short.pop_front();
				values_long.pop_front();
				values_long.push_back(src_series[i]);
				values_short.push_back(src_series[i]);
				dst_series[i] = sma_short - sma_long;
			}
		}
		return generated_name;
	}
}