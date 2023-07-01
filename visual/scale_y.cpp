#include "pch.hpp"
#include "scale_y.hpp"

//---------------------------------------------------------------------------------------------------------
autodis::visual::scale_y::scale_y(shared::math::range rng)
{
	if (rng.is_valid())
	{
		scale_ = (2.f * device_scale_ - top_margin_ - bottom_margin_) / rng;
		min_value_ = rng.min();
	}
}