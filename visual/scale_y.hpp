#pragma once

#include "framework.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class scale_y
	{
	private:
		static constexpr float device_scale_{1.}; // half of screen

		float bottom_margin_{0.01f};
		float top_margin_{0.01f};
		float scale_{std::numeric_limits<float>::signaling_NaN()};
		float min_value_{std::numeric_limits<float>::signaling_NaN()};

	public:
		void update(shared::math::min_max rng) noexcept;
		float position(double value) const noexcept { return -device_scale_ + bottom_margin_ + (static_cast<float>(value) - min_value_) * scale_; }
	};
}