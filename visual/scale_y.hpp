#pragma once

#include "framework.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class scale_y
	{
	private:
		static constexpr float device_scale_{1.};

		float bottom_margin_{0.01f};
		float top_margin_{0.01f};
		float scale_{0.f};
		float min_value_{0.f};

	public:
		scale_y(shared::math::range rng);

		float position(double value) const noexcept { return (static_cast<float>(value) - min_value_) * scale_; }
	};
}