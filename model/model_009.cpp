#include "pch.hpp"
#include "model_009.hpp"
#include "learn_runner.hpp"
#include "prediction_view.hpp"
#include <shared/math/target_delta.hpp>
#include <shared/math/track.hpp>
#include <shared/math/sma_delta.hpp>
#include <visual/heatmap.hpp>

//---------------------------------------------------------------------------------------------------------
autodis::model::model_009::model_009()
{
	norm_map_.reserve(feature_sources_count_ * track_depth_ + 1/*target*/);
}

//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::model_009::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	dr.read(
		{
			target_reg_id_open_,	// SBER open
			target_reg_id_high_,	// SBER high
			target_reg_id_low_,		// SBER low
			target_reg_id_close_,	// SBER close
			target_reg_id_vol_,		// SBER vol
			gold_reg_id_close_,		// GOLD close
			imoex_reg_id_close_		// IMOEX close
		},
		df_
	);
	assert(df_.col_count() == feature_sources_count_);
	norm_map_.resize(feature_sources_count_, norm_origin::null); 	// absolute values aren't used in learning so don't need normalization (set to 1)
	assert(norm_.empty());
}
//---------------------------------------------------------------------------------------------------------
// add computed target to df_
void autodis::model::model_009::_create_target()
{
	shared::math::target_delta_t0_t1(df_, target_source_df_idx_, df_);
	norm_map_.emplace_back(norm_origin::sber);
	assert(df_.col_count() == feature_sources_count_ + 1);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_create_features()
{
	input_series_names_.reserve(input_series_names_.size() + feature_sources_count_ * track_depth_ + 1/*sma_delta*/);

	for (size_t i{0}; i != feature_sources_count_; ++i)
	{
		std::vector<std::string> generated_names{
			shared::math::track(df_, i, track_depth_)};
		std::move(std::begin(generated_names), std::end(generated_names), std::back_inserter(input_series_names_));
 	}

	assert(df_.col_count() == feature_sources_count_ + 1 + feature_sources_count_ * track_depth_);

	input_series_names_.emplace_back(
		shared::math::sma_delta(df_, target_source_df_idx_, sma_period_short_, sma_period_long_));

	assert(df_.col_count() == feature_sources_count_ + 1 + feature_sources_count_ * track_depth_ + 1);

	// map normalizers for series of track features
	norm_map_.resize(norm_map_.size() + 4 * track_depth_, norm_origin::sber);		// track derived from sber ohlc
	norm_map_.resize(norm_map_.size() + track_depth_, norm_origin::sber_volume);	// track derived from sber volume
	norm_map_.resize(norm_map_.size() + track_depth_, norm_origin::gold);			// track derived from gold close
	norm_map_.resize(norm_map_.size() + track_depth_, norm_origin::imoex);			// track derived from imoex close

	norm_map_.resize(norm_map_.size() + 1, norm_origin::sber);						// sma_delta derived from sber close

	assert(norm_.empty());
}
//---------------------------------------------------------------------------------------------------------
// delete rows contaning nans
void autodis::model::model_009::_heal_data()
{
	df_ = df_.clear_lacunas(); // normalize should be called on lacune free data, so should clear frame not view till normalize is applied to the former
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_normalize()
{
	shared::math::min_max min_max_sber;
	shared::math::min_max min_max_sber_volume;
	shared::math::min_max min_max_gold;
	shared::math::min_max min_max_imoex;

	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		switch (norm_map_[i])
		{
		case norm_origin::null:			
			break;
		case norm_origin::sber:			
			min_max_sber += shared::math::min_max{df_.series(i)};
			break;
		case norm_origin::sber_volume:
			min_max_sber_volume += shared::math::min_max{df_.series(i)};
			break;
		case norm_origin::gold:			
			min_max_gold += shared::math::min_max{df_.series(i)};
			break;
		case norm_origin::imoex:			
			min_max_imoex += shared::math::min_max{df_.series(i)};
			break;
		}
	}

	norm_.clear();
	norm_.reserve(df_.col_count());

	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		switch (norm_map_[i])
		{
		case norm_origin::null:			
			norm_.emplace_back(norm_t{});
			break;
		case norm_origin::sber:			
			norm_.emplace_back(norm_t{min_max_sber});
			break;
		case norm_origin::sber_volume:
			norm_.emplace_back(norm_t{min_max_sber_volume});
			break;
		case norm_origin::gold:			
			norm_.emplace_back(norm_t{min_max_gold});
			break;
		case norm_origin::imoex:			
			norm_.emplace_back(norm_t{min_max_imoex});
			break;
		}
	}

	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		norm_[i].normalize(df_.series(i));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_create_chart()
{
	chart_ = std::make_shared<autodis::visual::chart>(df_);
	chart_->add_candlesticks(0, {0, 1, 2, 3});		// target candles. x-axis idx, {o h l c}
	chart_->add_line(1, df_.series_idx(target_series_name_), {0.f, 0.f, 1.f});		// target
	chart_->add_line(1, df_.series_idx(predicted_series_name_), {0.f, .5f, .5f});	// predicted
}
//---------------------------------------------------------------------------------------------------------
std::vector<std::size_t> autodis::model::model_009::_net_layer_sizes() const
{
	constexpr std::size_t input_size{feature_sources_count_ * track_depth_ + 1/*sma_delta*/};
//	assert(df_.col_count() == feature_sources_count_ + 1/*target*/ + input_size + 1/*predicted*/);
	return {input_size, 42, 42, 1};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_learn()
{
	shared::data::view dw{df_};
	learning::config mfn_cfg{_net_layer_sizes()};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::sample_filler input_filler{dw, input_series_names_, &mfn.input_layer()};
	learning::sample_filler target_filler{dw, {target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{input_filler, target_filler};
	auto predicted_series{dw.series_view(predicted_series_name_)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		mfn_cfg, mfn, teacher,
		input_filler, predicted_series, norm_[dw.series_idx(target_series_name_)], *chart_,
		[file_name = net_file_name_](nlohmann::json&& j){ std::ofstream(file_name) << j; }};
	runner.wait();
	best_err_ = runner.best_err();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_009::_predict()
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
	learning::sample_filler const input_filler{dw, input_series_names_, &mfn.input_layer()};

	input_filler.fill_last();
	mfn.forward();
	return prediction_result_t{dw.index_value(dw.row_count() - 1), mfn.omega_layer().front()};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_show()
{
	shared::data::view dw{df_};
	learning::config mfn_cfg{_net_layer_sizes()};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	{
		std::ifstream f(net_file_name_);
		nlohmann::json j = nlohmann::json::parse(f);
		j.get_to(mfn);
	}
	learning::sample_filler input_filler{dw, input_series_names_, &mfn.input_layer()};
	learning::sample_filler target_filler{dw, {target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{input_filler, target_filler};
	auto predicted_series{dw.series_view(predicted_series_name_)};
	autodis::prediction_view<learning::multilayer_feed_forward, norm_t> view{
		mfn, input_filler, predicted_series, norm_[dw.series_idx(target_series_name_)], *chart_};
	view.update_chart();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_print_df(frame_t const& df) const
{
	df.print_shape(std::cout);
	std::cout << "\n\n";
	std::ofstream f{"df.csv"s};
	df.print(f);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::learn()
{
	_load_data();
	_create_target();
	_create_features();
	_heal_data();
	_normalize();

	df_.create_series(predicted_series_name_);

	_create_chart();
	chart_->show();

	_print_df(df_);

	_learn();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_009::predict()
{
	_load_data();
	_create_features();
	_heal_data();
	_normalize();
	return _predict();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::show()
{
	_load_data();
	_create_target();
	_create_features();
	_heal_data();
	_normalize();
	df_.create_series(predicted_series_name_);
	_create_chart();
	chart_->show();
	_show();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::_show_analysis()
{
	learning::config mfn_cfg{_net_layer_sizes()};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	{
		std::ifstream f(net_file_name_);
		nlohmann::json j = nlohmann::json::parse(f);
		j.get_to(mfn);
	}

	auto const& weights{mfn.weight_layers()};

	if (weights.empty())
	{
		return;
	}

	std::ofstream out("network.txt", std::ios::trunc);
	auto const& source_to_dest_matrix{weights[0]};
	auto input_name_i{std::begin(input_series_names_)};
	for (std::vector<double> const& one_src_many_dst : source_to_dest_matrix)
	{
		out << *input_name_i++ << ", ";
		double sum_sqr{0};
		for (double weight : one_src_many_dst)
		{
			out << weight << ", ";
			sum_sqr += weight * weight;
		}
		out << std::sqrt(sum_sqr / source_to_dest_matrix.size()) << "\n";
	}

	visual::heatmap hm{weights[0], input_series_names_, net_file_name_};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_009::show_analysis()
{
	_load_data();
	_create_target();
	_create_features();
	_heal_data();
	_normalize();
	df_.create_series(predicted_series_name_);
	_show_analysis();
}
