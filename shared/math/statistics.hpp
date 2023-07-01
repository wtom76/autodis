#pragma once

#include "framework.hpp"

namespace shared::math
{
	//-----------------------------------------------------------------------------------------------------
	// class range
	//-----------------------------------------------------------------------------------------------------
	class range : private std::pair<double, double>
	{
	public:
		using pair<double, double>::pair;
		constexpr double min() const noexcept { return first; }
		constexpr double max() const noexcept { return second; }
		constexpr operator double() const noexcept { return second - first; }
		constexpr bool is_valid() const noexcept
		{
			return !std::isnan(first) && !std::isnan(second) && second - first >= std::numeric_limits<double>::epsilon();
		}
		static range min_max(const std::vector<double>& series) noexcept;
		static range min_max_nan(const std::vector<double>& series, size_t& nan_count) noexcept;
	};

	//-----------------------------------------------------------------------------------------------------
	inline double min(const std::vector<double>& series) noexcept
	{
		assert(!series.empty());
		double min_val{std::numeric_limits<double>::max()};
		for (const double val : series)
		{
			if (std::isnan(val))
			{
				continue;
			}
			if (val < min_val)
			{
				min_val = val;
			}
		}
		return min_val;
	}
	//-----------------------------------------------------------------------------------------------------
	inline double max(const std::vector<double>& series) noexcept
	{
		assert(!series.empty());
		double max_val{std::numeric_limits<double>::lowest()};
		for (const double val : series)
		{
			if (std::isnan(val))
			{
				continue;
			}
			if (val > max_val)
			{
				max_val = val;
			}
		}
		return max_val;
	}
	//-----------------------------------------------------------------------------------------------------
	inline range range::min_max(const std::vector<double>& series) noexcept
	{
		assert(!series.empty());
		double min_val{std::numeric_limits<double>::max()};
		double max_val{std::numeric_limits<double>::lowest()};
		for (const double val : series)
		{
			if (std::isnan(val))
			{
				continue;
			}
			if (val > max_val)
			{
				max_val = val;
			}
			if (val < min_val)
			{
				min_val = val;
			}
		}
		return {min_val, max_val};
	}
	//-----------------------------------------------------------------------------------------------------
	inline range range::min_max_nan(const std::vector<double>& series, size_t& nan_count) noexcept
	{
		assert(!series.empty());
		size_t nan_count_local = 0;
		double min_val{std::numeric_limits<double>::max()};
		double max_val{-std::numeric_limits<double>::max()};
		for (const double val : series)
		{
			if (std::isnan(val))
			{
				++nan_count_local;
				continue;
			}
			if (val > max_val)
			{
				max_val = val;
			}
			if (val < min_val)
			{
				min_val = val;
			}
		}
		nan_count = nan_count_local;
		return {min_val, max_val};
	}
}