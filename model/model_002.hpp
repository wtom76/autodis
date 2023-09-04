#pragma once

#include <shared/math/normalization.hpp>
#include <keeper/keeper.hpp>
#include "framework.hpp"

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
		using norm_t = shared::math::tanh_normalization;
		using norm_container_t = std::vector<norm_t>;
	// data
	private:
		frame_t				df_;
		frame_t				df_chart_;									// data for chart. no normalization
		norm_container_t	norm_;
		double				best_err_{0};
		size_t				original_series_count_{0};
		std::shared_ptr<autodis::visual::chart> chart_;

	// methods
	private:
		void _print_df(frame_t const& df) const;
		void _load_data();
		void _create_target();
		void _create_features();
		void _clear_data();
		void _normalize();
		void _create_chart();
		void _learn();

	public:
		void run();
	};
}