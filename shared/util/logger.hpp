#pragma once

#include "framework.hpp"
#include "scope_lifetime.hpp"
#include "file_system.hpp"

// In case of "Console logger error: async log: thread pool doens't exist anymore" probably
//    const util::SpdlogAsyncInit spdlog_init;
// is missed at the point of log creation.

namespace shared::util
{
	extern std::string const default_log_name;

	std::shared_ptr<spdlog::logger> create_logger(
		std::string const& name = default_log_name,
		std::string const& pattern = "%^%L %D %H:%M:%S:%e %v%$"s,
		size_t max_size = 8 * 1024 * 1024,
		size_t max_files = 12,
		bool console = false);

	std::shared_ptr<spdlog::logger> create_console_logger();

	//---------------------------------------------------------------------------------------------------------
	// An object should exist while logger lib is used
	// WARNING:
	// got an issue with spdlog::registry singleton address involved in calls from lambda
	// can differ from spdlog::registry singleton address involved in calls from main() and create_logger()
	// when create_logger() is compiled in static library cpp file as not inline
	// the issue was observed in Release build
	class spdlog_async_init : private scope_lifetime<void>
	{
	public:
		spdlog_async_init()
			: scope_lifetime<void>(
				[]()
				{
					// unkommenting this solves an issue described above, but mechanics of such behaviour is not clear
					//std::cout << "spdlog_async_init. reg: " << &spdlog::details::registry::instance() << std::endl; 
					spdlog::init_thread_pool(8192, 1);
				},
				[]()
				{
					spdlog::apply_all([](std::shared_ptr<spdlog::logger> log){ log->flush(); });
					spdlog::shutdown();
				}
			)
		{
		}
	};

	//---------------------------------------------------------------------------------------------------------
	// logged
	class logged
	{
	protected:
		std::shared_ptr<spdlog::logger> log_;

	protected:
		explicit logged(std::string const& name = default_log_name, std::string const& pattern = {})
			: log_{create_logger(name, pattern)}
		{}
		explicit logged(std::shared_ptr<spdlog::logger> log)
			: log_{log}
		{}
		~logged()
		{
			log_->flush();
		}

		spdlog::logger* log() const noexcept { return log_.get(); }
	};

	//---------------------------------------------------------------------------------------------------------
	// slow log getter for exceptional cases
	[[nodiscard]] inline std::shared_ptr<spdlog::logger> log() { return spdlog::get(default_log_name); }
}

//---------------------------------------------------------------------------------------------------------
inline std::shared_ptr<spdlog::logger> shared::util::create_logger(std::string const& name, std::string const& pattern,
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

		assert(spdlog::thread_pool());
		log = std::make_shared<spdlog::async_logger>(name, begin(sinks), end(sinks), spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);
		log->set_pattern(pattern);
		log->set_error_handler([](std::string const& msg)
			{
				std::cerr << "logger error: " << msg << std::endl;
			});
		spdlog::register_logger(log);

		log->set_level(spdlog::level::trace);

		SPDLOG_LOGGER_INFO(log, "~~~~~~~~~~~~~~~~~~~~~~~~");
		SPDLOG_LOGGER_INFO(log, "logger \"{}\" is created", log->name());
		SPDLOG_LOGGER_TRACE(log, "SPDLOG_LEVEL_TRACE");
		SPDLOG_LOGGER_DEBUG(log, "SPDLOG_LEVEL_DEBUG");
		SPDLOG_LOGGER_INFO(log, "SPDLOG_LEVEL_INFO");

		return log;
	}
	catch (std::exception const& ex)
	{
		std::cerr << "failed to create logger: " << ex.what() << std::endl;
		throw;
	}
}
//---------------------------------------------------------------------------------------------------------7
inline std::shared_ptr<spdlog::logger> shared::util::create_console_logger()
{
	std::shared_ptr<spdlog::logger> log{spdlog::get(default_log_name)};
	if (log)
	{
		return log;
	}
	log = create_logger(default_log_name, "%^%L %D %H:%M:%S:%e %v%$"s, 8 * 1024 * 1024, 12, true);

	return log;
}
