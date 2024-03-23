#include "pch.hpp"
#include "model/abstract.hpp"
#include "model/factory.hpp"
#include "config.hpp"
#include "learn_runner.hpp"
#include "application.hpp"

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
void autodis::application::create_model_file(std::string const& model_type, std::string const& model_name)
{
	model::factory::create_model_file(model_type, model_name);
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::learn(std::string const& model_name)
{
	std::unique_ptr<model::abstract> model{model::factory::make_unique(model_name)};
	if (model)
	{
		model->learn();
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::predict(std::string const& model_name)
{
	int64_t model_id{0};
	std::optional<model::prediction_result_t> result{};
	std::unique_ptr<model::abstract> model{model::factory::make_unique(model_name)};
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
	std::unique_ptr<model::abstract> model{model::factory::make_unique(model_name)};
	if (model)
	{
		model->show();
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::show_analysis(std::string const& model_name)
{
	std::unique_ptr<model::abstract> model{model::factory::make_unique(model_name)};
	if (model)
	{
		model->show_partial_dependence();
		//model->show_analysis();
	}
}
