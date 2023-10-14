#pragma once

#include <shared/math/normalization.hpp>
#include <keeper/keeper.hpp>
#include "framework.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class model_003
	// predict close_t1 - open_t1
	//---------------------------------------------------------------------------------------------------------
	class model_003
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = shared::math::tanh_normalization;
		using norm_container_t = std::vector<norm_t>;
		using prediction_result_t = std::pair<frame_t::index_value_t, frame_t::value_t>;
	// data
	private:
		std::string const	target_series_name_{"GAZP close(t+1)-GAZP open(t+1)"s};
		std::string const	predicted_series_name_{"predicted"s};
		std::string const	net_file_name_{"model_003.net.json"s};
		frame_t				df_;
		norm_container_t	norm_;
		double				best_err_{0};
		size_t				feature_sources_count_{0};	// number of first series that features will be created from
		std::shared_ptr<autodis::visual::chart> chart_;

	// methods
	private:
		std::vector<std::size_t> _net_layer_sizes() const;
		void _print_df(frame_t const& df) const;
		void _load_data();
		void _create_target();
		void _create_features();
		void _clear_data();
		void _normalize();
		void _create_chart();
		void _learn();
		std::optional<prediction_result_t> _predict();

	public:
		void learn();
		void predict();
	};
}