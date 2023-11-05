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
		void learn(std::string const& model_name);
		void predict(std::string const& model_name);
	};
}