#pragma once

#include <shared/math/normalization.hpp>
#include "framework.hpp"
#include "common.hpp"
#include "abstract.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class linear_test
	// testing model
	//---------------------------------------------------------------------------------------------------------
	class linear_test
		: public abstract
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
		std::shared_ptr<autodis::visual::chart> chart_;

	// methods
	private:
		void _load_data();
		void _create_target();
		void _normalize();
		void _create_chart();
		void _learn();

	public:
		std::int64_t id() override { return 1; }
		void learn() override;
		std::optional<prediction_result_t> predict() override;
		void show() override {}
		void show_analysis() override {}
		void show_partial_dependence() override {}
	};
}