#include "pch.hpp"
#include "scale_date.hpp"

//---------------------------------------------------------------------------------------------------------
autodis::visual::scale_date::scale_date(shared::data::frame const& df)
{
	if (df.index().size() < visible_count_)
	{
		visible_count_ = df.index().size();
		if (!visible_count_)
		{
			return;
		}
		scale_ = (2.f * device_scale_ - left_margin_ - right_margin_) / visible_count_;
	}
	first_visible_idx_ = df.index().size() - visible_count_;
}
//---------------------------------------------------------------------------------------------------------
float autodis::visual::scale_date::position(size_t idx) const noexcept
{
	assert(visible_count_ > 0);
	assert(idx >= first_visible_idx_);

	return -device_scale_ + left_margin_ + scale_ * (idx - first_visible_idx_);
}
