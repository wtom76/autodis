#pragma once

#include "framework.hpp"
#include "../data/frame.hpp"

namespace shared::math
{
	//---------------------------------------------------------------------------------------------------------
	// target_delta_t0_t1
	// creates x(t+1) - x(t)
	//---------------------------------------------------------------------------------------------------------
	inline void target_delta_t0_t1(data::frame const& src, std::size_t series_idx, data::frame& dst)
	{
		auto dst_i{dst.create_series(src.names()[series_idx] + "_delta(t+1)"s)->begin()};
		auto const src_e{src.series(series_idx).cend()};
		auto src_t1_i{src.series(series_idx).cbegin()};
		if (src_t1_i == src_e)
		{
			return;
		}
		auto src_t0_i{src_t1_i++};
		for (; src_t1_i != src_e; ++src_t1_i, ++src_t0_i, ++dst_i)
		{
			*dst_i = *src_t1_i - *src_t0_i;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	// target_delta_t0_t0
	// creates second(t+1) - first(t+1)
	//---------------------------------------------------------------------------------------------------------
	inline void target_delta_t0_t0(data::frame const& src, std::pair<std::size_t, std::size_t> series_idx, data::frame& dst)
	{
		std::string series_name{src.names()[series_idx.second] + "(t+1)-"s + src.names()[series_idx.first] + "(t+1)"};
		auto dst_i{dst.create_series(std::move(series_name))->begin()};
		auto const src_e{std::make_pair(src.series(series_idx.first).cend(), src.series(series_idx.second).cend())};
		auto src_t1_i{std::make_pair(src.series(series_idx.first).cbegin(), src.series(series_idx.second).cbegin())};
		if (src_t1_i.first == src_e.first || src_t1_i.second == src_e.second)
		{
			return;
		}
		++src_t1_i.first;
		++src_t1_i.second;
		for (; src_t1_i.first != src_e.first && src_t1_i.second != src_e.second; ++src_t1_i.first, ++src_t1_i.second, ++dst_i)
		{
			*dst_i = *src_t1_i.second - *src_t1_i.first;
		}
	}
}