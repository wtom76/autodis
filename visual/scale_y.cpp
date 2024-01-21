#include "pch.hpp"
#include "scale_y.hpp"

//---------------------------------------------------------------------------------------------------------
void autodis::visual::scale_y::update(shared::math::min_max rng) noexcept
{
	if (rng.valid())
	{
		float const new_scale{(2.f * device_scale_ - top_margin_ - bottom_margin_) / static_cast<float>(rng)};
		if (std::isnan(scale_) || scale_ > new_scale)
		{
			scale_ = new_scale;
		}
		if (std::isnan(min_value_) || min_value_ > rng.min())
		{
			min_value_ = rng.min();
		}
	}
}
