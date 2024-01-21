#pragma once

#include "framework.hpp"

namespace shared::math
{
	//-----------------------------------------------------------------------------------------------------
	// class min_max
	//-----------------------------------------------------------------------------------------------------
	class min_max
	{
	private:
		std::pair<double, double> pr_{0., 0.};
	public:
		min_max() = default;
		explicit min_max(double min, double max) noexcept;
		explicit min_max(std::pair<double, double> pr) noexcept;
		explicit min_max(std::span<double const> series) noexcept;
		constexpr double min() const noexcept { return pr_.first; }
		constexpr double max() const noexcept { return pr_.second; }
		constexpr operator double() const noexcept { return pr_.second - pr_.first; }
		constexpr bool valid() const noexcept
		{
			return !std::isnan(pr_.first) && !std::isnan(pr_.second) &&
					pr_.second - pr_.first >= std::numeric_limits<double>::epsilon();
		}
		min_max operator +=(min_max other) noexcept;
	};
	//-----------------------------------------------------------------------------------------------------
	inline min_max::min_max(double min, double max) noexcept
		: pr_{min, max}
	{}
	//-----------------------------------------------------------------------------------------------------
	inline min_max::min_max(std::pair<double, double> pr) noexcept
		: pr_{pr}
	{}
	//-----------------------------------------------------------------------------------------------------
	inline min_max::min_max(std::span<double const> series) noexcept
	{
		assert(!series.empty());
		double min_val{std::numeric_limits<double>::max()};
		double max_val{std::numeric_limits<double>::lowest()};
		for (double val : series)
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
		if (max_val - min_val >= std::numeric_limits<double>::epsilon())
		{
			pr_.first = min_val;
			pr_.second = max_val;
		}
	}
	//-----------------------------------------------------------------------------------------------------
	inline min_max min_max::operator +=(min_max other) noexcept
	{
		pr_.first = std::min(pr_.first, other.pr_.first);
		pr_.second = std::max(pr_.second, other.pr_.second);
		return *this;
	}
	//-----------------------------------------------------------------------------------------------------
	// NaN tolerant
	inline double min(std::span<double const> series) noexcept
	{
		assert(!series.empty());
		double min_val{std::numeric_limits<double>::max()};
		for (double val : series)
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
	// NaN tolerant
	inline double max(std::span<double const> series) noexcept
	{
		assert(!series.empty());
		double max_val{std::numeric_limits<double>::lowest()};
		for (double val : series)
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
}