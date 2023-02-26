#pragma once

#include <stop_token>
		
namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class progress_view
	// an interface to output learning progress
	//-----------------------------------------------------------------------------------------------------
	class progress_view
	{
	public:
		virtual std::stop_token stop_token() = 0;
		virtual void begin_teach() = 0;
		virtual void set_best(double min_err) = 0;
		virtual void set_last(double min_err) = 0;
		virtual void set_epoch(std::int64_t) = 0;
		virtual void end_teach() = 0;

		virtual void begin_test() = 0;
		virtual void add_sample_result(double result, double tagret) = 0;
		virtual void end_test(size_t true_count, size_t false_count) = 0;
	};
}
