#include "pch.hpp"
#include "linear_test.hpp"
#include "learn_runner.hpp"
#include "prediction_view.hpp"
#include "partial_dependence.hpp"
#include <visual/partial_dependence_view.hpp>

//---------------------------------------------------------------------------------------------------------
// class prediction_context
//---------------------------------------------------------------------------------------------------------
class autodis::model::linear_test::prediction_context
{
// data
private:
	shared::data::view					dw_;
	learning::config					mfn_cfg_;
	learning::multilayer_feed_forward	mfn_;
	learning::sample_filler				input_filler_;
public:
	explicit prediction_context(
		shared::data::frame& df)
		: dw_{df}
		, mfn_cfg_{{1, 3, 3, 3, 1}}
		, mfn_{mfn_cfg_}
		, input_filler_{dw_, {"x"s}, &mfn_.input_layer()}
	{}
	learning::config const& cfg() const noexcept { return mfn_cfg_; }
	shared::data::view& data_view() noexcept { return dw_; }
	learning::sample_filler& input_filler() noexcept { return input_filler_; }
	learning::multilayer_feed_forward& network() noexcept { return mfn_; }
};
//---------------------------------------------------------------------------------------------------------
// linear_test
//---------------------------------------------------------------------------------------------------------
autodis::model::linear_test::linear_test(file&& model_file)
	: model_file_{std::move(model_file)}
{}
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
	chart_ = std::make_shared<autodis::visual::chart>(df_);
	chart_->add_line(1, 1, {0.f, 0.f, 1.f});		// target
	chart_->add_line(1, 2, {0.f, .5f, .5f});		// predicted
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::_learn()
{
	prediction_context ctx{df_};
	learning::sample_filler target_filler{ctx.data_view(), {target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{ctx.input_filler(), target_filler};
	auto predicted_series{ctx.data_view().series_view(predicted_series_name_)};
	autodis::learn_runner<learning::multilayer_feed_forward, norm_t> runner{
		ctx.cfg(), ctx.network(), teacher,
		ctx.input_filler(), predicted_series, norm_[ctx.data_view().series_idx(target_series_name_)], *chart_,
		[this](nlohmann::json&& j){ model_file_.network() = std::move(j); model_file_.store(); }};
	runner.wait();
	best_err_ = runner.best_err();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::linear_test::_predict()
{
	assert(!model_file_.network().empty());

	if (!df_.row_count())
	{
		return {};
	}

	prediction_context ctx{df_};
	model_file_.network().get_to(ctx.network());
	ctx.input_filler().fill_last();
	ctx.network().forward();
	return prediction_result_t{
		ctx.data_view().index_value(ctx.data_view().row_count() - 1),
		norm_[1].restore(ctx.network().omega_layer().front())};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::_show()
{
	assert(!model_file_.network().empty());

	prediction_context ctx{df_};
	model_file_.network().get_to(ctx.network());
	learning::sample_filler target_filler{ctx.data_view(), {target_series_name_}, nullptr};
	learning::rprop<learning::multilayer_feed_forward> teacher{ctx.input_filler(), target_filler};
	auto predicted_series{ctx.data_view().series_view(predicted_series_name_)};
	autodis::prediction_view<learning::multilayer_feed_forward, norm_t> view{
		ctx.network(), ctx.input_filler(), predicted_series, norm_[ctx.data_view().series_idx(target_series_name_)], *chart_};
	view.update_chart();
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::create_model_file(
	std::string type_name,
	std::int64_t model_id,
	std::filesystem::path file_path)
{
	file f{type_name, model_id, file_path};
	f.store();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::learn()
{
	_load_data();
	_normalize();
	df_.create_series(predicted_series_name_);
	_create_chart();
	chart_->show();
	_learn();
}
//---------------------------------------------------------------------------------------------------------
std::optional<autodis::model::prediction_result_t> autodis::model::linear_test::predict()
{
	return {};
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::show()
{
	_load_data();
	_normalize();
	df_.create_series(predicted_series_name_);
	_create_chart();
	chart_->show();
	_show();
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::linear_test::show_partial_dependence()
{
	_load_data();
	_normalize();
	prediction_context ctx{df_};
	model_file_.network().get_to(ctx.network());
	partial_dependence<prediction_context> pd;
	pd.run(ctx, 100);

	{
		std::filesystem::path out_path{model_file_.path()};
		out_path.replace_extension("pd.csv");
		std::ofstream f{std::filesystem::path{out_path}};
		f << "series"sv << ';' << "min"sv << ';' << "max"sv << '\n';
		for (std::size_t col{0}; col != pd.result().size(); ++col)
		{
			auto const mm_pr{std::ranges::minmax(pd.result()[col])};
			f << input_series_names_[col] << ';' << mm_pr.min << ';' << mm_pr.max << '\n';
		}
		SPDLOG_LOGGER_INFO(log(), "result is written in {}", out_path.native());
	}
	visual::partial_dependence_view pdw{pd.result(), input_series_names_, "partial dependence"};
}
