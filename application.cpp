#include "pch.hpp"
#include "model/abstract.hpp"
#include "model/factory.hpp"
#include "config.hpp"
#include "learn_runner.hpp"
#include "application.hpp"
#include "feature/shop.hpp"
#include "feature/abstract.hpp"

//---------------------------------------------------------------------------------------------------------
static std::filesystem::path _model_file_path(std::string const& model_name)
{
	std::filesystem::path path{model_name};
	if (!path.has_extension())
	{
		path += ".json";
	}
	return path;
}

//----------------------------------------------------------------------------------------------------------
void autodis::application::_store_prediction(int64_t model_id, model::prediction_result_t const& result)
{
	try
	{
		pqxx::connection con{cfg().db_connection_};
		pqxx::work t{con};
		t.exec_params("call "s + cfg().store_prediction_ + "($1, $2, $3);"s, model_id, result.first, result.second);
		t.commit();
	}
	catch ([[maybe_unused]] std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to store prediction result. {}", ex.what());
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::create_model_file(std::string const& model_type, std::string const& out_file_name)
{
	std::filesystem::path const out_path{_model_file_path(out_file_name)};
	if (!shared::util::not_exist_or_overwrite(out_path))
	{
		return;
	}
	model::factory::create_model_file(model_type, _model_file_path(out_path));
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::learn(std::string const& model_name, std::string const& out_file_name)
{
	std::unique_ptr<model::abstract> model{model::factory::make_unique(_model_file_path(model_name))};
	if (model)
	{
		std::filesystem::path const out_path{_model_file_path(out_file_name)};
		if (!shared::util::not_exist_or_overwrite(out_path))
		{
			return;
		}
		model->learn(out_path);
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::predict(std::string const& model_name)
{
	int64_t model_id{0};
	std::optional<model::prediction_result_t> result{};
	std::unique_ptr<model::abstract> model{model::factory::make_unique(_model_file_path(model_name))};
	if (model)
	{
		model_id = model->id();
		result = model->predict();
	}
	if (result)
	{
		SPDLOG_LOGGER_INFO(log(), "{} {} {}", model_name, result.value().first, result.value().second);
		_store_prediction(model_id, result.value());
	}
	else
	{
		SPDLOG_LOGGER_INFO(log(), "{} n/a", model_name);
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::show(std::string const& model_name)
{
	std::unique_ptr<model::abstract> model{model::factory::make_unique(_model_file_path(model_name))};
	if (model)
	{
		model->show();
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::show_analysis(std::string const& model_name, std::filesystem::path const& out_path)
{
	std::unique_ptr<model::abstract> model{model::factory::make_unique(_model_file_path(model_name))};
	if (model)
	{
		model->show_partial_dependence(out_path);
		//model->show_analysis();
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::test_feature(std::int64_t feature_id, std::filesystem::path const& out_path)
{
	feature::shop shop;
	shop.feature(feature_id)->dump(out_path);
}
