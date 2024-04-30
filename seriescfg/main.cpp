#include "pch.hpp"
#include <shared/shared.hpp>
#include "config.hpp"
#include "series_params.hpp"
#include "op_create.hpp"
#include "op_delete.hpp"

using namespace seriescfg;

//---------------------------------------------------------------------------------------------------------
void print_help()
{
	SPDLOG_LOGGER_WARN(shared::util::log().get(), "usage: seriescfg <example|create|delete> <json filename>");
}

//---------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	shared::util::spdlog_async_init const spdlog_init;
	auto log{shared::util::create_console_logger()};

	try
	{
		config cfg;
		cfg.load();

		if (argc != 3)
		{
			print_help();
			return -1;
		}
		if ("example"s == argv[1])
		{
			series_params{}.store(argv[2]);
		}
		else if ("create"s == argv[1])
		{
			pqxx::connection con{cfg.db_connection_};
			op_create{series_params{argv[2]}}.run(con);
		}
		else if ("delete"s == argv[1])
		{
			pqxx::connection con{cfg.db_connection_};
			op_delete{series_params{argv[2]}}.run(con);
		}
		else
		{
			print_help();
			return -1;
		}
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_CRITICAL(log.get(), ex.what());
	}

	return 0;
}