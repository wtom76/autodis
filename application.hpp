#pragma once

#include "framework.hpp"
#include "model/common.hpp"

namespace autodis
{
	//----------------------------------------------------------------------------------------------------------
	// class application
	//----------------------------------------------------------------------------------------------------------
	class application final
		: private shared::util::logged
	{
	// types
	// data
	// methods
	private:
		void _store_prediction(int64_t model_id, model::prediction_result_t const& result);
	public:
		void create_model_file(std::string const& model_type, std::string const& model_name);
		void learn(std::string const& model_name);
		void predict(std::string const& model_name);
		void show(std::string const& model_name);
		void show_analysis(std::string const& model_name);
		void test_feature(std::int64_t feature_id, std::filesystem::path const& out_path);
	};
}