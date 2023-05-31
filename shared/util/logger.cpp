#include "pch.hpp"
#include "logger.hpp"
#include "file_system.hpp"

//---------------------------------------------------------------------------------------------------------
std::string const shared::util::default_log_name{"log"s};

//---------------------------------------------------------------------------------------------------------
std::shared_ptr<spdlog::logger> shared::util::create_logger(std::string const& name, std::string const& pattern,
	size_t max_size, size_t max_files, bool console)
{
	try
	{
		std::shared_ptr<spdlog::logger> log{spdlog::get(name)};
		if (log)
		{
			return log;
		}

		std::filesystem::path const log_dir{log_root() / exe_name()};
		std::filesystem::create_directories(log_dir);

		std::vector<spdlog::sink_ptr> sinks;
		if (console)
		{
			sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		}
		sinks.emplace_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>((log_dir / (name + ".log")).native(), max_size, max_files));

		log = std::make_shared<spdlog::async_logger>(name, begin(sinks), end(sinks), spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);
		log->set_level(spdlog::level::trace);
		log->set_pattern(pattern);
		log->set_error_handler([](std::string const& msg)
			{
				std::cerr << "Logger error: " << msg << std::endl;
			});
		spdlog::register_logger(log);

		return log;
	}
	catch (std::exception const& ex)
	{
		std::cerr << "Failed to create logger: " << ex.what() << std::endl;
		throw;
	}
}
//---------------------------------------------------------------------------------------------------------7
std::shared_ptr<spdlog::logger> shared::util::create_console_logger(const std::string& name)
{
	std::shared_ptr<spdlog::logger> log{spdlog::get(name)};
	if (log)
	{
		return log;
	}
	log = create_logger(name, "%^%D %H:%M:%S:%e %v%$"s, 8 * 1024 * 1024, 12, true);

	SPDLOG_LOGGER_INFO(log, "Logger has been created");
	SPDLOG_LOGGER_TRACE(log, "SPDLOG_LEVEL_TRACE");
	SPDLOG_LOGGER_DEBUG(log, "SPDLOG_LEVEL_DEBUG");
	SPDLOG_LOGGER_INFO(log, "SPDLOG_LEVEL_INFO");

	return log;
}
