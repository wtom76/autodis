#include "pch.hpp"
#include "learn_runner.hpp"
#include "model/linear_test.hpp"
#include "model/model_002.hpp"
#include "model/model_003.hpp"

#include "visual/chart.hpp"

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		shared::util::spdlog_async_init spdlog_init;
		auto log{shared::util::create_console_logger()};

		if (argc == 3 && "learn"s == argv[1])
		{
			if ("linear_test"s == argv[2])
			{
				autodis::model::linear_test{}.learn();
			}
			else if ("model_002"s == argv[2])
			{
				autodis::model::model_002{}.learn();
			}
			else if ("model_003"s == argv[2])
			{
				autodis::model::model_003{}.learn();
			}
		}
		else if (argc == 3 && "predict"s == argv[1])
		{
			if ("linear_test"s == argv[2])
			{
				autodis::model::linear_test{}.predict();
			}
			else if ("model_002"s == argv[2])
			{
				autodis::model::model_002{}.predict();
			}
			else if ("model_003"s == argv[2])
			{
				autodis::model::model_003{}.predict();
			}
		}
		else
		{
			std::cout << "usage:\n" << "autodis learn <model_name>\n" << "autodis predict <model_name>\n";
		}
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}