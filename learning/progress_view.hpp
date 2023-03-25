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
	{
		double min_err_{-1.};
	public:
		void begin_teach()
		{
			std::cout << "begin_teach\n";
		}
		void set_best(double min_err)
		{
			min_err_ = min_err;
			std::cout << "current best err: " << min_err << '\n';
		}
		void set_last(double min_err)
		{
			std::cout << "last err: " << min_err << ", min err: " << min_err_ << '\n';
		}
		void set_epoch(std::int64_t epoch)
		{
			std::cout << "epoch: " << epoch << '\n';
		}
		void end_teach()
		{
			std::cout << "end_teach\n";
		}

		void begin_test()
		{
			std::cout << "begin_test\n";
		}
		void add_sample_result(double result, double tagret)
		{
			std::cout << "result vs tagret: " << result << ", " << tagret
				<< ", abs err: " << result - tagret
				<< ", rel err: " << (tagret ? (result - tagret) / tagret : 0.)
				<< '\n';
		}
		void end_test(size_t true_count, size_t false_count)
		{
			std::cout << "end_test: hits vs errors:" << true_count << ", " << false_count << '\n';
		}
	};
}
