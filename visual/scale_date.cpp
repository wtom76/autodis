#include "pch.hpp"
#include "scale_date.hpp"

//---------------------------------------------------------------------------------------------------------
autodis::visual::scale_date::scale_date(shared::data::frame const& df)
	: end_idx_{df.index().size()}
{
	if (end_idx_ < visible_count_)
	{
		visible_count_ = end_idx_;
		if (!visible_count_)
		{
			return;
		}
		scale_ = (2.f * device_scale_ - left_margin_ - right_margin_) / visible_count_;
	}
	first_visible_idx_ = end_idx_ - visible_count_;
}
//---------------------------------------------------------------------------------------------------------
float autodis::visual::scale_date::position(size_t idx) const noexcept
{
	assert(visible_count_ > 0);
	assert(idx >= first_visible_idx_);

	return -device_scale_ + left_margin_ + scale_ * (idx - first_visible_idx_);
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::scale_date::start_scroll() noexcept
{
	fvi_on_scroll_start_ = first_visible_idx_;
}
//---------------------------------------------------------------------------------------------------------
bool autodis::visual::scale_date::scroll(float wnd_distance) noexcept
{
	ptrdiff_t const point_distance{static_cast<ptrdiff_t>(static_cast<float>(visible_count_default_) * wnd_distance)};
	if (!point_distance)
	{
		return false;
	}
	first_visible_idx_ = fvi_on_scroll_start_ - point_distance;
	if (first_visible_idx_ >= end_idx_)
	{
		first_visible_idx_ = end_idx_ - 1;
	}
	if (point_distance > static_cast<ptrdiff_t>(fvi_on_scroll_start_))
	{
		first_visible_idx_ = 0;
	}
	return true;
}
