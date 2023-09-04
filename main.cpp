#include "pch.hpp"
#include "learn_runner.hpp"
#include "model/linear_test.hpp"
#include "model/model_002.hpp"

#include "visual/chart.hpp"

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		shared::util::spdlog_async_init spdlog_init;
		auto log{shared::util::create_console_logger()};

		//autodis::model::linear_test{}.run();
		autodis::model::model_002{}.run();
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}