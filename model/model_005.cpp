#include "pch.hpp"
#include "model_005.hpp"
#include "learn_runner.hpp"
#include "prediction_view.hpp"
#include <shared/math/target_delta.hpp>
#include <shared/math/track.hpp>

//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::model_005::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	dr.read(
		{
			target_reg_id_close_,	// SBER close
			target_reg_id_high_,	// SBER high
			target_reg_id_low_,		// SBER low
			gold_reg_id_close_,		// GOLD close
			imoex_reg_id_close_,	// IMOEX close
			// for chart
			target_reg_id_open_	// SBER open
		},
		df_
	);
	feature_sources_count_ = 5;
}
//---------------------------------------------------------------------------------------------------------
// add computed target to df_
void autodis::model::model_005::_create_target()
{
	shared::math::target_delta_t0_t1(df_, 0, df_);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::_create_features()
{
	constexpr std::size_t track_depth{5};
	for (size_t i{0}; i != feature_sources_count_; ++i)
	{
		std::vector<std::string> generated_names{shared::math::track(df_, i, track_depth)};
		input_series_names_.reserve(input_series_names_.size() + generated_names.size());
		std::move(std::begin(generated_names), std::end(generated_names), std::back_inserter(input_series_names_));
 	}
}
//---------------------------------------------------------------------------------------------------------
// delete rows contaning nans
void autodis::model::model_005::_clear_data()
{
	df_ = df_.clear_lacunas(); // normalize should be called on lacune free data, so should clear frame not view till normalize is applied to the former
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::_normalize()
{
	norm_.clear();
	norm_.reserve(df_.col_count());
	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		norm_.emplace_back(shared::math::tanh_normalization{});
		norm_.back().normalize(df_.series(i));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::_create_chart()
{
	chart_ = std::make_shared<autodis::visual::chart>(df_);
	chart_->add_candlesticks(0, {5, 1, 2, 0});		// target candles. x-axis idx, {o h l c}
	chart_->add_line(1, df_.series_idx(target_series_name_), {0.f, 0.f, 1.f});		// target
	chart_->add_line(1, df_.series_idx(predicted_series_name_), {0.f, .5f, .5f});	// predicted
}
//---------------------------------------------------------------------------------------------------------
std::vector<std::size_t> autodis::model::model_005::_net_layer_sizes() const
{
	return {25, 50, 50, 1};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::_learn()
{
	shared::data::view dw{df_};
	learning::config mfn_cfg{_net_layer_sizes()};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::sample_filler const input_filler{dw, input_series_names_};
	learning::sample_filler const target_filler{dw, {target_series_name_}};
	learning::rprop<learning::multilayer_feed_forward> teacher{input_filler, target_filler};
	auto predicted_series{dw.series_view(predicted_series_name_)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		mfn_cfg, mfn, teacher,
		input_filler, predicted_series, norm_[dw.series_idx(target_series_name_)], *chart_,
		net_file_name_};
	runner.wait();
	best_err_ = runner.best_err();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_005::_predict()
{
	if (!df_.row_count())
	{
		return {};
	}
	shared::data::view dw{df_};
	learning::config mfn_cfg{_net_layer_sizes()};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	{
		std::ifstream f(net_file_name_);
		nlohmann::json j = nlohmann::json::parse(f);
		j.get_to(mfn);
	}
	learning::sample_filler const input_filler{dw, input_series_names_};

	input_filler.fill(dw.row_count() - 1, mfn.input_layer());
	mfn.forward();
	return prediction_result_t{dw.index_value(dw.row_count() - 1), mfn.omega_layer().front()};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::_show()
{
	shared::data::view dw{df_};
	learning::config mfn_cfg{_net_layer_sizes()};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	{
		std::ifstream f(net_file_name_);
		nlohmann::json j = nlohmann::json::parse(f);
		j.get_to(mfn);
	}
	learning::sample_filler const input_filler{dw, input_series_names_};
	learning::sample_filler const target_filler{dw, {target_series_name_}};
	learning::rprop<learning::multilayer_feed_forward> teacher{input_filler, target_filler};
	auto predicted_series{dw.series_view(predicted_series_name_)};
	autodis::prediction_view<learning::multilayer_feed_forward, norm_t> view{
		mfn, input_filler, predicted_series, norm_[dw.series_idx(target_series_name_)], *chart_};
	view.update_chart();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::_print_df(frame_t const& df) const
{
	df.print_shape(std::cout);
	std::cout << "\n\n";
	std::ofstream f{"df.csv"s};
	df.print(f);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::learn()
{
	_load_data();
	_create_target();
	_create_features();
	_clear_data();
	_normalize();

	df_.create_series(predicted_series_name_);

	_create_chart();
	chart_->show();

	_print_df(df_);

	_learn();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_005::predict()
{
	_load_data();
	_create_features();
	_clear_data();
	_normalize();
	return _predict();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_005::show()
{
	_load_data();
	_create_target();
	_create_features();
	_clear_data();
	_normalize();
	df_.create_series(predicted_series_name_);
	_create_chart();
	chart_->show();
	_show();
}
