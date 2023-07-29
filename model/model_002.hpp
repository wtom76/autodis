#pragma once

#include <shared/math/normalization.hpp>
#include "framework.hpp"
#include "visual/chart.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class model_002
	//---------------------------------------------------------------------------------------------------------
	class model_002
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = std::vector<shared::math::tanh_normalization>;
	// data
	private:
		frame_t	df_;
		frame_t	df_vis_;
		norm_t	norm_;
		double	best_err_{0};
		size_t	original_series_count_{0};

	// methods
	private:
		void _load_data();
		void _create_target();
		void _create_features();
		void _clear_data();
		void _normalize();
		void _learn();

	public:
		void run();
	};
}