#pragma once

#include "framework.hpp"

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// TODO: replace virtual functions by message queue
	//
	// class progress_view
	// learning progress output interface
	// to be implemented by callers
	//-----------------------------------------------------------------------------------------------------
	class progress_view : private shared::util::logged
	{
		double min_err_{-1.};
	public:
		double min_err() const noexcept
		{
			return min_err_;
		}
		void begin_teach()
		{
			SPDLOG_LOGGER_INFO(log(), "begin_teach");
		}
		void set_best(double min_err)
		{
			min_err_ = min_err;
			SPDLOG_LOGGER_INFO(log(), "current best mean sqr err: {}", min_err_);
		}
		void set_last(double min_err)
		{
			SPDLOG_LOGGER_INFO(log(), "last err: {}, min err: {}", min_err, min_err_);
		}
		void set_epoch(std::int64_t epoch)
		{
			SPDLOG_LOGGER_INFO(log(), "epoch: {}. ", epoch);
		}
		void end_teach()
		{
			SPDLOG_LOGGER_INFO(log(), "end_teach");
		}

		void begin_test()
		{
			SPDLOG_LOGGER_INFO(log(), "begin_test");;
		}
		void add_sample_result(double result, double tagret)
		{
			SPDLOG_LOGGER_INFO(log(), "result vs tagret: {}, {}, abs err: {}, rel err: {}",
				result, tagret, result - tagret, (tagret ? (result - tagret) / tagret : 0.));
		}
		void end_test(size_t true_count, size_t false_count)
		{
			SPDLOG_LOGGER_INFO(log(), "end_test: hits vs errors: {}, {}, best mean sqr err: {}",
				true_count, false_count,  min_err_);
		}
	};
}
