#pragma once

#include <shared/math/normalization.hpp>
#include "framework.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class model_000
	// testing model
	//---------------------------------------------------------------------------------------------------------
	class model_000
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = std::vector<shared::math::range_normalization>;
	// data
	private:
		frame_t	df_;
		norm_t	norm_;
		double	best_err_{0};

	// methods
	private:
		void _load_data();
		void _create_target();
		void _normalize();
		void _learn();

	public:
		void run();
	};
}