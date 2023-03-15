#pragma once

#include <iostream>

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
	public:
		void begin_teach()
		{
			std::cout << "begin_teach\n";
		}
		void set_best(double min_err)
		{
			std::cout << "current best err: " << min_err << '\n';
		}
		void set_last(double min_err)
		{
			std::cout << "last err: " << min_err << '\n';
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
			std::cout << "result vs tagret: " << result << ", " << tagret << '\n';
		}
		void end_test(size_t true_count, size_t false_count)
		{
			std::cout << "end_test: hits vs errors:" << true_count << ", " << false_count << '\n';
		}
	};
}
