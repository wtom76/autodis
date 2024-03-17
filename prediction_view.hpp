#pragma once
#include "framework.hpp"

namespace autodis
{
	//----------------------------------------------------------------------------------------------------------
	// class prediction_view
	//----------------------------------------------------------------------------------------------------------
	template <class net, class target_normalization = shared::math::tanh_normalization>
	class prediction_view
		: public learning::progress_view
	{
	public:
		using series_view_t = shared::data::view::series_view_t;
		using target_normalization_t = target_normalization;
		using chart_t = autodis::visual::chart;
	
	private:
		net&							network_;
		learning::sample_filler const*	inputs_filler_;
		series_view_t*					prediction_;
		target_normalization_t const	norm_;
		chart_t*						chart_{nullptr};

	public:
		//----------------------------------------------------------------------------------------------------------
		prediction_view(
			net& network,
			learning::sample_filler const& inputs_filler,
			series_view_t& prediction,
			target_normalization_t const& norm,
			chart_t& chart)
			: network_{network}
			, inputs_filler_{&inputs_filler}
			, prediction_{&prediction}
			, norm_{norm}
			, chart_{&chart}
		{
		}
		//----------------------------------------------------------------------------------------------------------
		~prediction_view()
		{}
		//----------------------------------------------------------------------------------------------------------
		void update_chart()
		{
			if (!chart_)
			{
				return;
			}
			assert(prediction_);
			assert(inputs_filler_);
			size_t row{0};
			for (double& dest : *prediction_)
			{
				inputs_filler_->fill(row++);
				network_.forward();
				dest = network_.omega_layer().front();
			}
			chart_->invalidate();
		}
	};
}
