#include "pch.hpp"
#include "model_011.hpp"
#include "learn_runner.hpp"
#include "prediction_view.hpp"
#include "partial_dependence.hpp"
#include <shared/math/target_delta.hpp>
#include <shared/math/track.hpp>
#include <shared/math/sma_delta.hpp>

//---------------------------------------------------------------------------------------------------------
// class prediction_context
//---------------------------------------------------------------------------------------------------------
class autodis::model::model_011::prediction_context
{
// data
private:
	shared::data::view					dw_;
	learning::config					mfn_cfg_;
	learning::multilayer_feed_forward	mfn_;
	learning::sample_filler				input_filler_;

private:
	static learning::config&& _set_input_layer_size(learning::config& net_cfg, std::size_t size) noexcept
	{
		net_cfg.set_input_layer_size(size);
		return std::move(net_cfg);
	}
public:
	explicit prediction_context(
		shared::data::frame& df,
		learning::config&& net_cfg,
		std::vector<std::string> const& input_series_names)
		: dw_{df}
		, mfn_cfg_{_set_input_layer_size(net_cfg, input_series_names.size())}
		, mfn_{mfn_cfg_}
		, input_filler_{dw_, input_series_names, &mfn_.input_layer()}
	{}
	learning::config const& net_cfg() const noexcept { return mfn_cfg_; }
	shared::data::view& data_view() noexcept { return dw_; }
	learning::sample_filler& input_filler() noexcept { return input_filler_; }
	learning::multilayer_feed_forward& network() noexcept { return mfn_; }
};
//---------------------------------------------------------------------------------------------------------
// model_011
//---------------------------------------------------------------------------------------------------------
autodis::model::model_011::model_011(file&& model_file)
	: shop_{std::make_unique<feature::shop>(model_file.feature_set_id())}
	, model_file_{std::move(model_file)}
{}
//---------------------------------------------------------------------------------------------------------
autodis::model::model_011::~model_011()
{}
//---------------------------------------------------------------------------------------------------------
learning::config autodis::model::model_011::_config()
{
	learning::config cfg;
	model_file_.net_config().get_to(cfg);
	return cfg;
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_print_df(frame_t const& df, std::filesystem::path const& path) const
{
	df.print_shape(std::cout);
	std::cout << "\n\n";
	std::ofstream f{path};
	df.print(f);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_fill_normalized(feature::abstract& src_feature, shared::data::frame::series_t& dst_df_series)
{
	for (std::size_t df_idx{0}; df_idx != df_.row_count(); ++df_idx)
	{
		if (df_.index()[df_idx] < src_feature.bounds().index_min_)
		{
			continue;
		}
		if (df_.index()[df_idx] > src_feature.bounds().index_max_)
		{
			break;
		}

		dst_df_series[df_idx] = src_feature.norm().normalize(src_feature.value(df_.index()[df_idx]));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_create_target()
{
	assert(!df_.col_count());
	feature::master_index const& mi{shop_->index()};
	std::shared_ptr<feature::abstract> target{shop_->feature(model_file_.target().get<std::int64_t>())};
	target_series_name_ = target->label();
	target_norm_ = target->norm();
	shared::data::frame::series_t& df_series{df_.create_series(target->label())};
	shared::data::frame::index_t& df_index{df_.index()};
	feature::index_pos_t mi_pos{mi.pos(target->bounds().index_min_)};
	feature::index_pos_t const mi_pos_max{mi.pos(target->bounds().index_max_)};
	df_.reserve(mi_pos_max - mi_pos + 1);
	for (; mi_pos <= mi_pos_max; ++mi_pos)
	{
		feature::index_value_t mival{mi.at(mi_pos)};
		df_index.emplace_back(mival);
		df_series.emplace_back(target_norm_.normalize(target->value(mival)));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_create_input_features()
{
	for (nlohmann::json& fj : model_file_.features())
	{
		_create_input_feature(fj);
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_create_input_feature(nlohmann::json& fj)
{
	std::shared_ptr<feature::abstract> feature{shop_->feature(fj)};
	if (!feature)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature:\n{}", fj.dump());
		throw std::runtime_error{"failed to create feature"s};
	}
	assert(!feature->info().is_template());
	if (!feature->info().id_)
	{
		to_json(fj, feature->info());
	}
	
	std::string label{"i"s + std::to_string(input_series_names_.size() + 1) + '.' + feature->label()};
	shared::data::frame::series_t& df_series{df_.create_series(label)};
	input_series_names_.emplace_back(std::move(label));
	_fill_normalized(*feature, df_series);
}
//---------------------------------------------------------------------------------------------------------
// 1. init df shape with target
// 2. fill inputs
// 3. clear lacunes
void autodis::model::model_011::_create_features()
{
	df_.clear();

	// 1.
	_create_target();
	if (!df_.row_count())
	{
		return;
	}
	// 2.
	_create_input_features();

// DEBUG
	_print_df(df_, "df_raw.csv"sv);
//~DEBUG
	// 3.
	df_ = df_.clear_lacunas(); // normalize should be called on lacune free data, so should clear frame not view till normalize is applied to the former
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_create_chart()
{
	chart_ = std::make_shared<autodis::visual::chart>(df_);
	chart_->add_line(1, df_.series_idx(target_series_name_), {0.f, 0.f, 1.f});	// target
	chart_->add_line(1, df_.series_idx(predicted_series_name_), {0.f, .5f, .5f});	// predicted
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_learn(std::filesystem::path const& out_path)
{
	prediction_context ctx{df_, _config(), input_series_names_};
	learning::sample_filler target_filler{ctx.data_view(), {target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{ctx.input_filler(), target_filler};
	auto predicted_series{ctx.data_view().series_view(predicted_series_name_)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		ctx.net_cfg(), ctx.network(), teacher,
		ctx.input_filler(), predicted_series, target_norm_, *chart_,
		[this](learning::multilayer_feed_forward const& network)
		{
			model_file_.network() = network;
		}
	};
	runner.wait();
	model_file_.error() = runner.best_err();
	model_file_.store(out_path);
	SPDLOG_LOGGER_INFO(log(), "model with error {} is stored to '{}'", runner.best_err(), out_path.native());
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_011::_predict()
{
	assert(!model_file_.network().empty());

	if (!df_.row_count())
	{
		return {};
	}
	prediction_context ctx{df_, _config(), input_series_names_};
	model_file_.network().get_to(ctx.network());
	ctx.input_filler().fill_last();
	ctx.network().forward();
	return prediction_result_t{
		ctx.data_view().index_value(ctx.data_view().row_count() - 1),
		target_norm_.restore(ctx.network().omega_layer().front())};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_show()
{
	assert(!model_file_.network().empty());
	prediction_context ctx{df_, _config(), input_series_names_};
	model_file_.network().get_to(ctx.network());
	learning::sample_filler target_filler{ctx.data_view(), {target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{ctx.input_filler(), target_filler};
	auto predicted_series{ctx.data_view().series_view(predicted_series_name_)};
	autodis::prediction_view<learning::multilayer_feed_forward, norm_t> view{
		ctx.network(), ctx.input_filler(), predicted_series, target_norm_, *chart_};
	view.update_chart();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::_show_analysis()
{
	assert(!model_file_.network().empty());

	prediction_context ctx{df_, _config(), input_series_names_};
	model_file_.network().get_to(ctx.network());

	auto const& weights{ctx.network().weight_layers()};

	if (weights.empty())
	{
		return;
	}

	std::ofstream out{"network_analysis.txt", std::ios::trunc};

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
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::create_model_file(
	std::string type_name,
	std::int64_t model_id,
	std::filesystem::path const& out_path)
{
	file f{type_name, model_id};
	{
		learning::config default_net_cfg{{0, 16, 8, 1}};
		f.net_config() = default_net_cfg;
	}
	f.store(out_path);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::learn(std::filesystem::path const& out_path)
{
	_create_features();

	model_file_.store(out_path);

	df_.create_series(predicted_series_name_);

	_create_chart();
	chart_->show();

	_print_df(df_, "df.csv"sv);

	_learn(out_path);
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::model_011::predict()
{
	_create_features();
	return _predict();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::show()
{
	_create_features();
	df_.create_series(predicted_series_name_);
	_create_chart();
	chart_->show();
	_show();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::show_analysis()
{
	_create_features();
	df_.create_series(predicted_series_name_);
	_show_analysis();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_011::show_partial_dependence(std::filesystem::path const& out_path)
{
	_create_features();
	prediction_context ctx{df_, _config(), input_series_names_};
	model_file_.network().get_to(ctx.network());
	partial_dependence<prediction_context> pd;
	pd.run(ctx, 100);

	{
		std::ofstream f{out_path};
		pd.write_sorted_result(f, input_series_names_);
		SPDLOG_LOGGER_INFO(log(), "result is written in {}", out_path.native());
	}
}
