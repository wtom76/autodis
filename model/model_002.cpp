#include "pch.hpp"
#include "model_002.hpp"
#include "learn_runner.hpp"
#include <shared/math/target_delta.hpp>
#include <shared/math/track.hpp>

//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::model_002::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	dr.read(
		{
			4,	// GAZP close
			11,	// GOLD close
			16	// IMOEX close
		},
		df_
	);
	original_series_count_ = df_.col_count();
	assert(original_series_count_ == 3);

	dr.read(
		{
			4,	// GAZP close
			8,	// GOLD open
			9,	// GOLD high
			10,	// GOLD low
			11,	// GOLD close
		},
		df_chart_
	);
}
//---------------------------------------------------------------------------------------------------------
// add computed target to df_
void autodis::model::model_002::_create_target()
{
	shared::math::target_delta(df_, 0, df_);
	shared::math::target_delta(df_chart_, 0, df_chart_);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_create_features()
{
	constexpr std::size_t track_depth{5};
	for (size_t i{0}; i != original_series_count_; ++i)
	{
		shared::math::track(df_, i, track_depth);
	}
}
//---------------------------------------------------------------------------------------------------------
// delete rows contaning nans
void autodis::model::model_002::_clear_data()
{
	df_ = df_.clear_lacunas(); // normalize should be called on lacune free data, so should clear frame not view till normalize is applied to the former
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_normalize()
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
void autodis::model::model_002::_create_chart()
{
	chart_ = std::make_shared<autodis::visual::chart>(df_chart_);
	chart_->add_candlesticks(0, {1, 2, 3, 4});		// gold candles (aka inputs)
	chart_->add_line(1, 5, {0.f, 0.f, 1.f});		// target
	chart_->add_line(1, 6, {0.f, .5f, .5f});		// predicted
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_learn()
{
	std::vector<std::size_t> const layers_sizes{15, 30, 30, 1};
	shared::data::view dw{df_};

	learning::config mfn_cfg{layers_sizes};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::sample_filler const input_filler{dw,
	{
		"GAZP close_delta(t-1)"s,
		"GAZP close_delta(t-2)"s,
		"GAZP close_delta(t-3)"s,
		"GAZP close_delta(t-4)"s,
		"GAZP close_delta(t-5)"s,
		"GOLD close_delta(t-1)"s,
		"GOLD close_delta(t-2)"s,
		"GOLD close_delta(t-3)"s,
		"GOLD close_delta(t-4)"s,
		"GOLD close_delta(t-5)"s,
		"IMOEX close_delta(t-1)"s,
		"IMOEX close_delta(t-2)"s,
		"IMOEX close_delta(t-3)"s,
		"IMOEX close_delta(t-4)"s,
		"IMOEX close_delta(t-5)"s
	}};
	learning::sample_filler const target_filler{dw,
	{
		"GAZP close_delta(t+1)"s
	}};
	learning::rprop<learning::multilayer_feed_forward> teacher{input_filler, target_filler};
	shared::data::view dw_vis{df_chart_};
	auto predicted_series{dw_vis.series_view("predicted"s)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		mfn_cfg, mfn, teacher,
		input_filler, predicted_series, norm_[dw.series_idx("GAZP close_delta(t+1)"s)],
		*chart_};
	runner.wait();
	best_err_ = runner.best_err();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_print_df(frame_t const& df) const
{
	df.print_shape(std::cout);
	std::cout << "\n\n";
	std::ofstream f{"df.csv"s};
	df.print(f);
}
//---------------------------------------------------------------------------------------------------------
// learn
void autodis::model::model_002::run()
{
	_load_data();

	_create_target();
	_create_features();

	_clear_data();
	_normalize();

	_print_df(df_);

	df_chart_.create_series("predicted"s);

	_print_df(df_chart_);

	_create_chart();
	chart_->show();

	_learn();
}