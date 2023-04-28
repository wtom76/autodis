#pragma once

#include "framework.hpp"
#include "statistics.hpp"

namespace shared::math
{
	//-----------------------------------------------------------------------------------------------------
	enum class normalization_method : int
	{
		null, sigmoid, tanh
	};

	//-----------------------------------------------------------------------------------------------------
	// Normalize to normal_range
	inline void normalize_range(range original_range, range normal_range, std::vector<double>& series)
	{
		assert(normal_range.is_valid());
		if (!original_range.is_valid())
		{
			throw std::runtime_error("normalize_range: original range is nan or less than epsilon");
		}
		const double a{normal_range / original_range};
		const double b{original_range.min()};
		const double normal_b{normal_range.min()};
		transform(cbegin(series), cend(series), begin(series),
			[a, b, normal_b](double val)
			{
				return a * (val - b) + normal_b;
			}
		);
	}
	//-----------------------------------------------------------------------------------------------------
	// Normalize to range [-1;1] keeping sign
	inline void normalize_tanh(range original_range, std::vector<double>& series)
	{
		if (!original_range.is_valid())
		{
			throw std::runtime_error("normalize_tanh: original range is nan or less than epsilon");
		}
		if (abs(original_range.max()) > abs(original_range.min()))
		{
			assert(original_range.max() > 0.);
			original_range = range{-original_range.max(), original_range.max()};
		}
		else
		{
			assert(original_range.min() < 0.);
			original_range = range{original_range.min(), -original_range.min()};
		}
		normalize_range(original_range, range{-1., 1.}, series);
	}

	//-----------------------------------------------------------------------------------------------------
	// class normalization
	//-----------------------------------------------------------------------------------------------------
	class normalization
	{
	public:
		virtual void normalize(std::vector<double>& series) = 0;
		virtual double restore(double normalized_val) const = 0;
	};

	//-----------------------------------------------------------------------------------------------------
	// class range_normalization
	//-----------------------------------------------------------------------------------------------------
	class range_normalization : public normalization
	{
		// data
	private:
		range normal_range_;
		range original_range_;
		range current_range_;

		// methods
	private:
	public:
		explicit range_normalization(const range& normal_range) : normal_range_(normal_range) {}
		void normalize(std::vector<double>& series) override;
		double restore(double normalized_val) const override;
	};
	//-----------------------------------------------------------------------------------------------------
	inline void range_normalization::normalize(std::vector<double>& series)
	{
		original_range_ = range::min_max(series);
		normalize_range(original_range_, normal_range_, series);
		current_range_ = normal_range_;
	}
	//-----------------------------------------------------------------------------------------------------
	inline double range_normalization::restore(double normalized_val) const
	{
		return (normalized_val - current_range_.min()) * original_range_ / current_range_
			+ original_range_.min();
	}

	//-----------------------------------------------------------------------------------------------------
	// class lognormal
	//-----------------------------------------------------------------------------------------------------
	class lognormal : public normalization
	{
		// methods
	public:
		void normalize(std::vector<double>& series) override;
		double restore(double normalized_val) const override;
	};
	//-----------------------------------------------------------------------------------------------------
	inline void lognormal::normalize(std::vector<double>& series)
	{
		std::transform(std::cbegin(series), std::cend(series), std::begin(series),
			[](auto val)
			{
				assert(val > 0.);
				return std::log(val);
			}
		);
	}
	//-----------------------------------------------------------------------------------------------------
	inline double lognormal::restore(double normalized_val) const
	{
		return std::exp(normalized_val);
	}
}