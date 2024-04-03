#pragma once

#include "framework.hpp"
#include "statistics.hpp"

namespace shared::math
{
	////-----------------------------------------------------------------------------------------------------
	//enum class normalization_method : int
	//{
	//	null, sigmoid, tanh
	//};

	//-----------------------------------------------------------------------------------------------------
	// Normalize to normal_range
	inline void normalize_range(min_max original_range, min_max normal_range, std::vector<double>& series)
	{
		assert(normal_range.valid());
		if (!original_range.valid())
		{
			throw std::runtime_error("normalize_range: original min_max is nan or less than epsilon");
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

	////-----------------------------------------------------------------------------------------------------
	//// class normalization
	////-----------------------------------------------------------------------------------------------------
	//class normalization
	//{
	//public:
	//	virtual void normalize(std::span<double>& series) = 0;
	//	virtual double restore(double normalized_val) const = 0;
	//};

	//-----------------------------------------------------------------------------------------------------
	// class range_normalization
	//-----------------------------------------------------------------------------------------------------
	//class range_normalization : public normalization
	//{
	//	// data
	//private:
	//	min_max normal_range_;
	//	min_max original_range_;
	//	min_max current_range_;

	//	// methods
	//private:
	//public:
	//	explicit range_normalization(min_max normal_range) : normal_range_(normal_range)
	//	{}
	//	void normalize(std::vector<double>& series) override;
	//	double restore(double normalized_val) const override;
	//};
	////-----------------------------------------------------------------------------------------------------
	//inline void range_normalization::normalize(std::vector<double>& series)
	//{
	//	original_range_ = min_max(series);
	//	normalize_range(original_range_, normal_range_, series);
	//	current_range_ = normal_range_;
	//}
	////-----------------------------------------------------------------------------------------------------
	//inline double range_normalization::restore(double normalized_val) const
	//{
	//	return (normalized_val - current_range_.min()) * original_range_ / current_range_
	//		+ original_range_.min();
	//}

	//-----------------------------------------------------------------------------------------------------
	// class tanh_normalization
	// Normalize to min_max [-1;1] preserving sign
	//-----------------------------------------------------------------------------------------------------
	class tanh_normalization
	{
		// data
	private:
		double factor_{1.};

		// methods
	private:
		//-----------------------------------------------------------------------------------------------------
		static double _factor(double original_max)
		{
			if (!std::isnormal(original_max))
			{
				throw std::runtime_error("normalize_tanh: original range is not valid");
			}
			return 1. / original_max;
		}
		//-----------------------------------------------------------------------------------------------------
		static double _factor(min_max rng)
		{
			return _factor(std::max(std::abs(rng.max()), std::abs(rng.min())));
		}
	public:
		//-----------------------------------------------------------------------------------------------------
		tanh_normalization() noexcept = default;
		tanh_normalization(tanh_normalization const&) noexcept = default;
		tanh_normalization& operator = (tanh_normalization const&) noexcept = default;
		tanh_normalization(tanh_normalization&&) noexcept = default;
		tanh_normalization& operator = (tanh_normalization&&) noexcept = default;
		//-----------------------------------------------------------------------------------------------------
		explicit tanh_normalization(min_max original_range) noexcept
			: factor_{_factor(original_range)}
		{}
		//-----------------------------------------------------------------------------------------------------
		explicit tanh_normalization(double original_max) noexcept
			: factor_{_factor(original_max)}
		{}
		//-----------------------------------------------------------------------------------------------------
		double normalize(double original_val) const noexcept
		{
			return original_val * factor_;
		}
		//-----------------------------------------------------------------------------------------------------
		double restore(double normalized_val) const noexcept
		{
			return normalized_val / factor_;
		}
		//-----------------------------------------------------------------------------------------------------
		void normalize(std::vector<double>& series) const noexcept
		{
			for (double& dest : series)
			{
				dest *= factor_;
			}
		}
	};
	////-----------------------------------------------------------------------------------------------------
	//// class lognormal
	////-----------------------------------------------------------------------------------------------------
	//class lognormal : public normalization
	//{
	//	// methods
	//public:
	//	void normalize(std::vector<double>& series) override;
	//	double restore(double normalized_val) const override;
	//};
	////-----------------------------------------------------------------------------------------------------
	//inline void lognormal::normalize(std::vector<double>& series)
	//{
	//	std::transform(std::cbegin(series), std::cend(series), std::begin(series),
	//		[](auto val)
	//		{
	//			assert(val > 0.);
	//			return std::log(val);
	//		}
	//	);
	//}
	////-----------------------------------------------------------------------------------------------------
	//inline double lognormal::restore(double normalized_val) const
	//{
	//	return std::exp(normalized_val);
	//}
}