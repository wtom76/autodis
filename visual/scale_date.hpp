#pragma once

#include "framework.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class scale_date
	{
	private:
		static constexpr float device_scale_{1.};			// half of screen
		static constexpr size_t visible_count_default_{100};

		float left_margin_{0.01};
		float right_margin_{0.01};
		float scale_{(2.f * device_scale_ - left_margin_ - right_margin_) / visible_count_default_};
		size_t first_visible_idx_{0};
		size_t visible_count_{visible_count_default_};

	public:
		explicit scale_date(shared::data::frame const& df);

		size_t first_visible_idx() const noexcept { return first_visible_idx_; }
		float position(size_t idx) const noexcept;
		float step() const noexcept { return scale_; }
	};
}