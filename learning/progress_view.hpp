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

	private:
		virtual void _begin_teach() {}
		virtual void _set_best() {}
		virtual void _set_last(double min_err) {}
		virtual void _set_epoch() {}
		virtual void _end_teach() {}
		virtual void _begin_test() {}
		virtual void _add_sample_result(double result, double tagret) {}
		virtual void _end_test(size_t true_count, size_t false_count, size_t total_count) {}

	public:
		virtual ~progress_view(){}

		double best_err() const noexcept
		{
			return best_err_;
		}
		void begin_teach()
		{
			SPDLOG_LOGGER_INFO(log(), "begin_teach");
			_begin_teach();
		}
		void set_best(double min_err)
		{
			best_err_ = min_err;
			SPDLOG_LOGGER_INFO(log(), "current best mean sqr err: {}", best_err_);
			_set_best();
		}
		void set_last(double min_err)
		{
			SPDLOG_LOGGER_INFO(log(), "epoch: {}. last err: {}, min err: {}", epoch_, min_err, best_err_);
			_set_last(min_err);
		}
		void set_epoch(std::int64_t epoch)
		{
			epoch_ = epoch;
			_set_epoch();
		}
		void end_teach()
		{
			SPDLOG_LOGGER_INFO(log(), "end_teach");
			_end_teach();
		}
		void begin_test()
		{
			SPDLOG_LOGGER_INFO(log(), "begin_test");;
			_begin_test();
		}
		void add_sample_result(double result, double tagret)
		{
			SPDLOG_LOGGER_INFO(log(), "result vs tagret: {}, {}, abs err: {}, rel err: {}",
				result, tagret, result - tagret, (tagret ? (result - tagret) / tagret : 0.));
			_add_sample_result(result, tagret);
		}
		void end_test(size_t true_count, size_t false_count, size_t total_count)
		{
			SPDLOG_LOGGER_INFO(log(), "end_test: hits/misses/total: {}/{}/{}, best mean sqr err: {}",
				true_count, false_count, total_count, best_err_);
			_end_test(true_count, false_count, total_count);
		}
	};
}
