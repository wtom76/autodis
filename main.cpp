#include "pch.hpp"
#include "config.hpp"
#include "application.hpp"

namespace po = boost::program_options;

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	shared::util::spdlog_async_init spdlog_init;
	auto logger{shared::util::create_console_logger()};
	try
	{
		std::string command;
		std::string file_name;

		po::options_description all_options{"autodis options"};
		all_options.add_options()
			("command,C", po::value<std::string>(&command))
			("file,F", po::value<std::string>(&file_name), "model file name")
			("type,T", po::value<std::string>(), "model type")
			("feature_id,I", po::value<std::int64_t>(), "feature DB id")
			("out,O", po::value<std::string>(), "output file name");
		po::positional_options_description p;
		p.add("command", 1).add("file", 1);
		po::variables_map params;
		po::store(po::command_line_parser(argc, argv).options(all_options).positional(p).run(), params);
		po::notify(params);

		autodis::cfg().load();
		autodis::application app;

		if (command == "makefile"sv)
		{
			app.create_model_file(params["type"s].as<std::string>(), file_name);
		}
		else if (command == "learn"sv)
		{
			app.learn(file_name);
		}
		else if (command == "predict"sv)
		{
			app.predict(file_name);
		}
		else if (command == "show"sv)
		{
			app.show(file_name);
		}
		else if (command == "show_network"sv)
		{
			app.show_analysis(file_name);
		}
		else if (command == "test_feature"sv)
		{
			std::string const out_path{params["out"s].as<std::string>()};
			app.test_feature(params["feature_id"s].as<std::int64_t>(), out_path);
		}
		else
		{
			std::cout
				<< all_options
				<< "usage:\n"
				<< "autodis makefile <model_name> --type=<model_type>\n"
				<< "autodis learn <model_name>\n"
				<< "autodis predict <model_name>\n"
				<< "autodis show <model_name>\n"
				<< "autodis show_network <model_name>\n"
				<< "autodis test_feature --feature_id=<id> --out=<path>\n";
		}
	}
	catch ([[maybe_unused]] std::exception const& ex)
	{
		SPDLOG_LOGGER_CRITICAL(logger.get(), ex.what());
		return 1;
	}
	return 0;
}