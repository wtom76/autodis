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

		if (argc == 4 && "makefile"s == argv[1])
		{
			app.create_model_file(argv[2], argv[3]);
		}
		else if (argc == 3 && "learn"s == argv[1])
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
		else if (argc == 4 && "show"s == argv[1] && "network"s == argv[2])
		{
			app.show_analysis(argv[3]);
		}
		else
		{
			std::cout
				<< "usage:\n"
				<< "autodis makefile <model_type> <model_name>\n"
				<< "autodis learn <model_name>\n"
				<< "autodis predict <model_name>\n"
				<< "autodis show <model_name>\n"
				<< "autodis show network <model_name>\n";
		}
	}
	catch ([[maybe_unused]] std::exception const& ex)
	{
		SPDLOG_LOGGER_CRITICAL(logger.get(), ex.what());
		return 1;
	}
	return 0;
}