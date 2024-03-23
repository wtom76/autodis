#pragma once

#include <shared/math/normalization.hpp>
#include <keeper/keeper.hpp>
#include "framework.hpp"
#include "common.hpp"
#include "abstract.hpp"
#include "file.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class config_010
	// model config
	//---------------------------------------------------------------------------------------------------------
	class config_010
	{
	// types
	public:
		//---------------------------------------------------------------------------------------------------------
		class target
		{
		public:
			long long	reg_id_open_{0};
			long long	reg_id_high_{0};
			long long	reg_id_low_{0};
			long long	reg_id_close_{0};
			long long	reg_id_vol_{0};
		};
		//---------------------------------------------------------------------------------------------------------
		using ids_t = std::vector<long long>;
		using layer_sizes_t = std::vector<std::size_t>;

	// data
	public:
		layer_sizes_t	layer_sizes_;
		target			target_;
		ids_t			other_reg_ids_;
		std::string		target_series_name_{"<symbol> close_delta(t+1)"s};

	// methods
	public:
		std::size_t target_source_num() const noexcept { return sizeof(target) / sizeof(long long); }
		std::size_t other_source_num() const noexcept { return other_reg_ids_.size(); }
		std::size_t source_num() const noexcept { return sizeof(target) / sizeof(long long) + other_reg_ids_.size(); }
		ids_t all_reg_ids() const;
	};
	//---------------------------------------------------------------------------------------------------------
	// class prediction_context
	//---------------------------------------------------------------------------------------------------------
	class prediction_context
	{
	// data
	private:
		shared::data::view					dw_;
		learning::config					mfn_cfg_;
		learning::multilayer_feed_forward	mfn_;
		learning::sample_filler				input_filler_;
	public:
		explicit prediction_context(
			shared::data::frame& df,
			config_010 const& cfg,
			std::vector<std::string> const& input_series_names)
			: dw_{df}
			, mfn_cfg_{cfg.layer_sizes_}
			, mfn_{mfn_cfg_}
			, input_filler_{dw_, input_series_names, &mfn_.input_layer()}
		{}
		learning::config const& cfg() const noexcept { return mfn_cfg_; }
		shared::data::view& data_view() noexcept { return dw_; }
		learning::sample_filler& input_filler() noexcept { return input_filler_; }
		learning::multilayer_feed_forward& network() noexcept { return mfn_; }
	};
	//---------------------------------------------------------------------------------------------------------
	// class model_010
	// predict close_t1 - close_t0
	//---------------------------------------------------------------------------------------------------------
	class model_010
		: public abstract
		, private shared::util::logged
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = shared::math::tanh_normalization;

		enum class norm_origin : std::size_t
		{
			null = 0,
			target,
			target_volume,
			other_first
		};

		using ids_t = std::vector<long long>;
		using norm_map_t = std::vector<std::underlying_type_t<norm_origin>>;
		using norm_container_t = std::vector<norm_t>;

	// data
	private:
		static constexpr std::size_t	target_source_df_idx_{3};	// an index of source series in df_ target will be made from (close price)
		static constexpr std::size_t	track_depth_{20};
		static constexpr std::size_t	sma_period_short_{20};
		static constexpr std::size_t	sma_period_long_{50};

		std::string const	predicted_series_name_{"predicted"s};

		file				model_file_;
		config_010			cfg_;
		frame_t				df_;
		norm_container_t	norm_;
		norm_map_t			norm_map_;
		double				best_err_{0};
		std::shared_ptr<autodis::visual::chart> chart_;
		std::vector<std::string> input_series_names_;

	// methods
	private:
		static void _set_layer_sizes_default(config_010& cfg);
		void _print_df(frame_t const& df) const;
		void _load_data();
		void _create_target();
		void _create_features();
		void _heal_data();
		void _normalize();
		void _create_chart();
		void _learn();
		std::optional<prediction_result_t> _predict();
		void _show();
		void _show_analysis();

	public:
		model_010(file&& model_file);

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