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
	class progress_view
		: protected shared::util::logged
	{
	protected:
		double best_err_{-1.};
		std::int64_t epoch_{0};

	public:
		double best_err() const noexcept
		{
			return best_err_;
		}
		virtual void begin_teach()
		{
			SPDLOG_LOGGER_INFO(log(), "begin_teach");
		}
		virtual void set_best(double min_err)
		{
			best_err_ = min_err;
			SPDLOG_LOGGER_INFO(log(), "current best mean sqr err: {}", best_err_);
		}
		virtual void set_last(double min_err)
		{
			SPDLOG_LOGGER_INFO(log(), "epoch: {}. last err: {}, min err: {}", epoch_, min_err, best_err_);
		}
		virtual void set_epoch(std::int64_t epoch)
		{
			epoch_ = epoch;
		}
		virtual void end_teach()
		{
			SPDLOG_LOGGER_INFO(log(), "end_teach");
		}

		virtual void begin_test()
		{
			SPDLOG_LOGGER_INFO(log(), "begin_test");;
		}
		virtual void add_sample_result(double result, double tagret)
		{
			SPDLOG_LOGGER_INFO(log(), "result vs tagret: {}, {}, abs err: {}, rel err: {}",
				result, tagret, result - tagret, (tagret ? (result - tagret) / tagret : 0.));
		}
		virtual void end_test(size_t true_count, size_t false_count, size_t total_count)
		{
			SPDLOG_LOGGER_INFO(log(), "end_test: hits/misses/total: {}/{}/{}, best mean sqr err: {}",
				true_count, false_count, total_count, best_err_);
		}
	};
}
