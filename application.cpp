#include "pch.hpp"
#include "model/linear_test.hpp"
#include "model/model_008.hpp"
#include "model/model_009.hpp"
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
void autodis::application::learn(std::string const& model_name)
{
	if ("linear_test"s == model_name)
	{
		autodis::model::linear_test{}.learn();
	}
	else if ("model_008"s == model_name)
	{
		autodis::model::model_008{}.learn();
	}
	else if ("model_009"s == model_name)
	{
		autodis::model::model_009{}.learn();
	}
}
//----------------------------------------------------------------------------------------------------------
void autodis::application::predict(std::string const& model_name)
{
	int64_t model_id{0};
	std::optional<model::prediction_result_t> result{};

	if ("linear_test"s == model_name)
	{
		model_id = 1;
		result = autodis::model::linear_test{}.predict();
	}
	else if ("model_008"s == model_name)
	{
		model_id = 8;
		result = autodis::model::model_008{}.predict();
	}
	else if ("model_009"s == model_name)
	{
		model_id = 9;
		result = autodis::model::model_009{}.predict();
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
	if ("model_008"s == model_name)
	{
		autodis::model::model_008{}.show();
	}
	else if ("model_009"s == model_name)
	{
		autodis::model::model_009{}.show();
	}
}