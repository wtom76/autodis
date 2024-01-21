#include "pch.hpp"
#include "linear_test.hpp"
#include "learn_runner.hpp"

//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::linear_test::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	dr.read(
		{
			6,	// test_linear/x
			7	// test_linear/y
		},
		df_
	);
	df_chart_ = df_;
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::_normalize()
{
	norm_.clear();
	norm_.reserve(df_.col_count());
	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		norm_.emplace_back(norm_t{shared::math::min_max{df_.series(i)}});
		norm_.back().normalize(df_.series(i));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::_create_chart()
{
	chart_ = std::make_shared<autodis::visual::chart>(df_chart_);
	chart_->add_line(1, 1, {0.f, 0.f, 1.f});		// target
	chart_->add_line(1, 2, {0.f, .5f, .5f});		// predicted
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::_learn()
{
	std::vector<std::size_t> const layers_sizes{1, 3, 3, 3, 1};
	shared::data::view dw{df_};

	learning::config mfn_cfg{layers_sizes};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::sample_filler const input_filler{dw, {"x"s}};
	learning::sample_filler const target_filler{dw, {"y"s}};
	learning::rprop<learning::multilayer_feed_forward> teacher{input_filler, target_filler};
	shared::data::view dw_vis{df_chart_};
	auto predicted_series{dw_vis.series_view("predicted"s)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		mfn_cfg, mfn, teacher,
		input_filler, predicted_series, norm_[dw.series_idx("y"s)], *chart_,
		"linear_test.net.json"s};
	runner.wait();
	best_err_ = runner.best_err();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::learn()
{
	_load_data();
	_normalize();
	df_chart_.create_series("predicted"s);
	_create_chart();
	chart_->show();
	_learn();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::linear_test::predict()
{
	return {};
}
