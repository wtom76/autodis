#pragma once

#include <shared/math/normalization.hpp>
#include <keeper/keeper.hpp>
#include <feature/abstract.hpp>
#include <feature/shop.hpp>
#include "framework.hpp"
#include "common.hpp"
#include "abstract.hpp"
#include "file.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class model_011
	// predict close_t1 - close_t0
	//---------------------------------------------------------------------------------------------------------
	class model_011
		: public abstract
		, private shared::util::logged
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = shared::math::tanh_normalization;
		using ids_t = std::vector<long long>;

		class prediction_context;

	public:
		class config
		{
		public:
			learning::config net_cfg_;
			config() : net_cfg_{{0, 16, 8, 1}}
			{}
		};

	// data
	private:
		std::string const predicted_series_name_{"predicted"s};

		std::unique_ptr<feature::shop> shop_;
		file		model_file_;
		config		cfg_;
		frame_t		df_;
		double		best_err_{0};
		std::shared_ptr<autodis::visual::chart> chart_;
		std::string					target_series_name_;
		std::vector<std::string>	input_series_names_;
		feature::abstract::normalization_t target_norm_;

	// methods
	private:
		static void _set_layer_sizes_default(config& cfg);
		void _adjust_cfg_input_size();
		void _print_df(frame_t const& df, std::filesystem::path const& path) const;
		void _fill_normalized(feature::abstract& src_feature, shared::data::frame::series_t& dst_df_series);
		void _create_target();
		void _create_input_features();
		void _create_input_feature(nlohmann::json& fj);
		void _create_features();
		void _create_chart();
		void _learn();
		std::optional<prediction_result_t> _predict();
		void _show();
		void _show_analysis();

	public:
		model_011(file&& model_file);

		static void create_model_file(
			std::string type_name,
			std::int64_t model_id,
			std::filesystem::path file_path);

		std::int64_t id() override { return model_file_.model_id(); }
		void learn() override;
		std::optional<prediction_result_t> predict() override;
		void show() override;
		void show_analysis() override;
		void show_partial_dependence() override;
	};
}