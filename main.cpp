#include "pch.hpp"
#include "config.hpp"
#include "application.hpp"

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	shared::util::spdlog_async_init spdlog_init;
	auto logger{shared::util::create_console_logger()};
	try
	{
		autodis::cfg().load();
		autodis::application app;

		if (argc == 3 && "learn"s == argv[1])
		{
			app.learn(argv[2]);
		}
		else if (argc == 3 && "predict"s == argv[1])
		{
			app.predict(argv[2]);
		}
		else if (argc == 3 && "show"s == argv[1])
		{
			app.show(argv[2]);
		}
		else
		{
			std::cout << "usage:\n" << "autodis learn <model_name>\n" << "autodis predict <model_name>\n";
		}
	}
	catch ([[maybe_unused]] std::exception const& ex)
	{
		SPDLOG_LOGGER_CRITICAL(logger.get(), ex.what());
		return 1;
	}
	return 0;
}