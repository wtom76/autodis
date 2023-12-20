#pragma once

#include <shared/math/normalization.hpp>
#include <keeper/keeper.hpp>
#include "framework.hpp"
#include "common.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class model_006
	// predict close_t1 - close_t0
	//---------------------------------------------------------------------------------------------------------
	class model_006
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = shared::math::tanh_normalization;
		using norm_container_t = std::vector<norm_t>;
		
	// data
	private:
		static constexpr long long target_reg_id_open_{18};
		static constexpr long long target_reg_id_high_{19};
		static constexpr long long target_reg_id_low_{20};
		static constexpr long long target_reg_id_close_{21};
		static constexpr long long target_reg_id_vol_{22};
		static constexpr long long gold_reg_id_close_{11};
		static constexpr long long imoex_reg_id_close_{16};

		static constexpr std::size_t target_df_idx_{3};			// an index of target series in df_
		static constexpr std::size_t track_depth_{5};

		std::string const	target_series_name_{"SBER close_delta(t+1)"s};
		std::string const	predicted_series_name_{"predicted"s};
		std::string const	net_file_name_{"model_006.net.json"s};
		frame_t				df_;
		norm_container_t	norm_;
		double				best_err_{0};
		size_t				feature_sources_count_{0};	// number of first series that features will be created from
		std::shared_ptr<autodis::visual::chart> chart_;
		std::vector<std::string> input_series_names_;

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
		void _show();

	public:
		void learn();
		std::optional<prediction_result_t> predict();
		void show();
	};
}