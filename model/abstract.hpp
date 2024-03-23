#pragma once

#include "common.hpp"

namespace autodis::model
{
	class abstract
	{
	public:
		virtual ~abstract() = default;
		virtual std::int64_t id() = 0;
		virtual void learn() = 0;
		virtual std::optional<prediction_result_t> predict() = 0;
		virtual void show() = 0;
		virtual void show_analysis() = 0;
		virtual void show_partial_dependence() = 0;
	};
}