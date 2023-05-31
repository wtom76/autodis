#pragma once

#include "framework.hpp"
#include "scope_lifetime.hpp"

// In case of "Console logger error: async log: thread pool doens't exist anymore" probably
//    const util::SpdlogAsyncInit spdlog_init;
// is missed before log creation.

namespace shared::util
{
	extern std::string const default_log_name;

	std::shared_ptr<spdlog::logger> create_logger(const std::string& name, const std::string& pattern = "%v",
		size_t max_size = 8 * 1024 * 1024, size_t max_files = 12, bool console = false);

	std::shared_ptr<spdlog::logger> create_console_logger(const std::string& name = default_log_name);

	//---------------------------------------------------------------------------------------------------------
	// An object should exist while logger lib is used
	class spdlog_async_init : private scope_lifetime<void>
	{
	public:
		spdlog_async_init()
			: scope_lifetime<void>(
				[]()
				{
					spdlog::init_thread_pool(8192, 1);
				},
				[]()
				{
					spdlog::apply_all([](std::shared_ptr<spdlog::logger> log){ log->flush(); });
					spdlog::shutdown();
				}
			)
		{}
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