#include "pch.hpp"
#include "model_010.hpp"
#include "learn_runner.hpp"
#include "prediction_view.hpp"
#include "partial_dependence.hpp"
#include <shared/math/target_delta.hpp>
#include <shared/math/track.hpp>
#include <shared/math/sma_delta.hpp>
#include <visual/heatmap.hpp>
#include <visual/partial_dependence_view.hpp>
#include <visual/box_plot.hpp>

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// config_010
	//---------------------------------------------------------------------------------------------------------
	void to_json(nlohmann::json& j, config_010::target const& src)
	{
		j = nlohmann::json{
			{ "open",	src.reg_id_open_ },
			{ "high",	src.reg_id_high_ },
			{ "low",	src.reg_id_low_ },
			{ "close",	src.reg_id_close_ },
			{ "vol",	src.reg_id_vol_ }
		};
	}
	//---------------------------------------------------------------------------------------------------------
	void from_json(nlohmann::json const& j, config_010::target& dst)
	{
		j.at("open").get_to(dst.reg_id_open_);
		j.at("high").get_to(dst.reg_id_high_);
		j.at("low").get_to(dst.reg_id_low_);
		j.at("close").get_to(dst.reg_id_close_);
		j.at("vol").get_to(dst.reg_id_vol_);
	}
	//---------------------------------------------------------------------------------------------------------
	void to_json(nlohmann::json& j, config_010 const& src)
	{
		j = nlohmann::json{
			{ "layer_sizes",		src.layer_sizes_ },
			{ "target_reg_ids",		src.target_ },
			{ "other_reg_ids",		src.other_reg_ids_ },
			{ "target_series_name",	src.target_series_name_ }
		};
	}
	//---------------------------------------------------------------------------------------------------------
	void from_json(nlohmann::json const& j, config_010& dst)
	{
		j.at("layer_sizes").get_to(dst.layer_sizes_);
		j.at("target_reg_ids").get_to(dst.target_);
		j.at("other_reg_ids").get_to(dst.other_reg_ids_);
		j.at("target_series_name").get_to(dst.target_series_name_);
	}
}
//---------------------------------------------------------------------------------------------------------
autodis::model::config_010::ids_t autodis::model::config_010::all_reg_ids() const
{
	ids_t result;
	result.reserve(source_num());
	result.emplace_back(target_.reg_id_open_);
	result.emplace_back(target_.reg_id_high_);
	result.emplace_back(target_.reg_id_low_);
	result.emplace_back(target_.reg_id_close_);
	result.emplace_back(target_.reg_id_vol_);
	std::ranges::copy(other_reg_ids_, std::back_inserter(result));
	return result;
}
//---------------------------------------------------------------------------------------------------------
// class prediction_context
//---------------------------------------------------------------------------------------------------------
class autodis::model::model_010::prediction_context
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
// model_010
//---------------------------------------------------------------------------------------------------------
autodis::model::model_010::model_010(file&& model_file)
	: model_file_{std::move(model_file)}
	, cfg_{model_file_.config().get<config_010>()}
{}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_set_layer_sizes_default(config_010& cfg)
{
	cfg.layer_sizes_ = {0, 16, 8, 1};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_adjust_cfg_input_size()
{
	assert(!cfg_.layer_sizes_.empty());
	cfg_.layer_sizes_[0] = input_series_names_.size();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_print_df(frame_t const& df) const
{
	df.print_shape(std::cout);
	std::cout << "\n\n";
	std::ofstream f{"df.csv"s};
	df.print(f);
}
//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::model_010::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	dr.read(cfg_.all_reg_ids(), df_);
	assert(df_.col_count() == cfg_.source_num());
	norm_map_.resize(cfg_.source_num(), std::to_underlying(norm_origin::null)); 	// absolute values aren't used in learning so don't need normalization (set to null)
	assert(norm_.empty());
}
//---------------------------------------------------------------------------------------------------------
// add computed target to df_
void autodis::model::model_010::_create_target()
{
	shared::math::target_delta_t0_t1(df_, target_source_df_idx_, df_);
	norm_map_.emplace_back(std::to_underlying(norm_origin::target));
	assert(df_.col_count() == cfg_.source_num() + 1);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_create_features()
{
	input_series_names_.reserve(input_series_names_.size() + cfg_.source_num() * track_depth_ + 1/*sma_delta*/);

	for (size_t i{0}; i != cfg_.source_num(); ++i)
	{
		std::vector<std::string> generated_names{shared::math::track(df_, i, track_depth_)};
		std::ranges::move(generated_names, std::back_inserter(input_series_names_));
 	}

	input_series_names_.emplace_back(
		shared::math::sma_delta(df_, target_source_df_idx_, sma_period_short_, sma_period_long_));

	// map normalizers for series of track features
	norm_map_.resize(norm_map_.size() + (cfg_.target_source_num() - 1) * track_depth_, std::to_underlying(norm_origin::target));	// track derived from target ohlc
	norm_map_.resize(norm_map_.size() + track_depth_, std::to_underlying(norm_origin::target_volume));								// track derived from target volume
	for (size_t i{0}; i != cfg_.other_source_num(); ++i)
	{
		norm_map_.resize(norm_map_.size() + track_depth_, std::to_underlying(norm_origin::other_first) + i);						// track derived from other sources
	}
	norm_map_.resize(norm_map_.size() + 1, std::to_underlying(norm_origin::target));												// sma_delta derived from target close

	assert(norm_.empty());
}
//---------------------------------------------------------------------------------------------------------
// delete rows contaning nans
void autodis::model::model_010::_heal_data()
{
	df_ = df_.clear_lacunas(); // normalize should be called on lacune free data, so should clear frame not view till normalize is applied to the former
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_normalize()
{
	shared::math::min_max min_max_target;
	shared::math::min_max min_max_target_vol;
	std::vector<shared::math::min_max> min_max_other(cfg_.other_source_num());

	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		switch (norm_map_[i])
		{
		case std::to_underlying(norm_origin::null):			
			break;
		case std::to_underlying(norm_origin::target):			
			min_max_target += shared::math::min_max{df_.series(i)};
			break;
		case std::to_underlying(norm_origin::target_volume):
			min_max_target_vol += shared::math::min_max{df_.series(i)};
			break;
		default:
			assert(norm_map_[i] - std::to_underlying(norm_origin::other_first) < min_max_other.size());
			min_max_other[norm_map_[i] - std::to_underlying(norm_origin::other_first)] += shared::math::min_max{df_.series(i)};
			break;
		}
	}

	norm_.clear();
	norm_.reserve(df_.col_count());

	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		switch (norm_map_[i])
		{
		case std::to_underlying(norm_origin::null):
			norm_.emplace_back(norm_t{});
			break;
		case std::to_underlying(norm_origin::target):
			norm_.emplace_back(norm_t{min_max_target});
			break;
		case std::to_underlying(norm_origin::target_volume):
			norm_.emplace_back(norm_t{min_max_target_vol});
			break;
		default:
			norm_.emplace_back(norm_t{min_max_other[norm_map_[i] - std::to_underlying(norm_origin::other_first)]});
			break;
		}
	}

	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		norm_[i].normalize(df_.series(i));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_create_chart()
{
	chart_ = std::make_shared<autodis::visual::chart>(df_);
	chart_->add_candlesticks(0, {0, 1, 2, 3});		// target candles. x-axis idx, {o h l c}
	chart_->add_line(1, df_.series_idx(cfg_.target_series_name_), {0.f, 0.f, 1.f});	// target
	chart_->add_line(1, df_.series_idx(predicted_series_name_), {0.f, .5f, .5f});	// predicted
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_learn()
{
	_adjust_cfg_input_size();
	prediction_context ctx{df_, cfg_, input_series_names_};
	learning::sample_filler target_filler{ctx.data_view(), {cfg_.target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{ctx.input_filler(), target_filler};
	auto predicted_series{ctx.data_view().series_view(predicted_series_name_)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		ctx.cfg(), ctx.network(), teacher,
		ctx.input_filler(), predicted_series, norm_[ctx.data_view().series_idx(cfg_.target_series_name_)], *chart_,
		[this](nlohmann::json&& j){ model_file_.network() = std::move(j); model_file_.store(); }};
	runner.wait();
	best_err_ = runner.best_err();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_010::_predict()
{
	assert(!model_file_.network().empty());
	assert(norm_map_[cfg_.source_num()] == std::to_underlying(norm_origin::target));

	if (!df_.row_count())
	{
		return {};
	}

	_adjust_cfg_input_size();
	prediction_context ctx{df_, cfg_, input_series_names_};
	model_file_.network().get_to(ctx.network());
	ctx.input_filler().fill_last();
	ctx.network().forward();
	return prediction_result_t{
		ctx.data_view().index_value(ctx.data_view().row_count() - 1),
		norm_[cfg_.source_num()].restore(ctx.network().omega_layer().front())};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_show()
{
	assert(!model_file_.network().empty());

	_adjust_cfg_input_size();
	prediction_context ctx{df_, cfg_, input_series_names_};
	model_file_.network().get_to(ctx.network());
	learning::sample_filler target_filler{ctx.data_view(), {cfg_.target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{ctx.input_filler(), target_filler};
	assert(norm_map_[ctx.data_view().series_idx(cfg_.target_series_name_)] == std::to_underlying(norm_origin::target));
	auto predicted_series{ctx.data_view().series_view(predicted_series_name_)};
	autodis::prediction_view<learning::multilayer_feed_forward, norm_t> view{
		ctx.network(), ctx.input_filler(), predicted_series, norm_[ctx.data_view().series_idx(cfg_.target_series_name_)], *chart_};
	view.update_chart();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::_show_analysis()
{
	assert(!model_file_.network().empty());

	_adjust_cfg_input_size();
	prediction_context ctx{df_, cfg_, input_series_names_};
	model_file_.network().get_to(ctx.network());

	auto const& weights{ctx.network().weight_layers()};

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

	visual::heatmap hm{weights[0], input_series_names_, model_file_.path().filename()};
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::create_model_file(
	std::string type_name,
	std::int64_t model_id,
	std::filesystem::path file_path)
{
	file f{type_name, model_id, file_path};
	{
		config_010 default_cfg{};
		_set_layer_sizes_default(default_cfg);
		f.config() = default_cfg;
	}
	f.store();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::learn()
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
std::optional<autodis::model::prediction_result_t> autodis::model::model_010::predict()
{
	_load_data();
	_create_features();
	_heal_data();
	_normalize();
	return _predict();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::show()
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
void autodis::model::model_010::show_analysis()
{
	_load_data();
	_create_target();
	_create_features();
	_heal_data();
	_normalize();
	df_.create_series(predicted_series_name_);
	_show_analysis();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_010::show_partial_dependence()
{
	_load_data();
	_create_features();
	_heal_data();
	_normalize();
	_adjust_cfg_input_size();
	prediction_context ctx{df_, cfg_, input_series_names_};
	model_file_.network().get_to(ctx.network());
	partial_dependence<prediction_context> pd;
	pd.run(ctx, 100);

	{
		std::filesystem::path out_path{model_file_.path()};
		out_path.replace_extension("pd.csv");
		std::ofstream f{out_path};
		pd.write_sorted_result(f, input_series_names_);
		SPDLOG_LOGGER_INFO(log(), "result is written in {}", out_path.native());
	}
	visual::partial_dependence_view pdw{pd.result(), input_series_names_, "partial dependence"};

	//visual::box_plot pdw{pd.result(), input_series_names_, "partial dependence"};
}
