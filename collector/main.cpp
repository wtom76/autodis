#include "pch.hpp"
#include <curl/curl.h>
#include <keeper/keeper.hpp>
#include <shared/shared.hpp>
#include "source/file.hpp"
#include "factory.hpp"

//---------------------------------------------------------------------------------------------------------
void process_source(keeper::metadata::source_info const& info, keeper::config const& keeper_cfg)
{
	assert(info.pending_);
	auto src{collector::factory::source(info.source_uri_, info.source_args_)};
	auto feed{collector::factory::feed(info.dest_)};
	feed->start(std::make_unique<keeper::data_write>(keeper_cfg, info.dest_.data_uri_));
	src->fetch_to(*feed);

	SPDLOG_LOGGER_INFO(shared::util::log(), "processed source '{}' ({})", info.source_uri_, info.source_args_);
}
//---------------------------------------------------------------------------------------------------------
void collect()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::metadata metadata{keeper_cfg};
	const auto sources{metadata.load()};
	for (auto const& source_info : sources)
	{
		if (!source_info.pending_)
		{
			continue;
		}
		try
		{
			process_source(source_info, keeper_cfg);
			metadata.drop_pending_flag(source_info.source_id_);
		}
		catch (std::exception const& ex)
		{
			SPDLOG_LOGGER_ERROR(shared::util::log(), ex.what());
		}
	}
}
//---------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		shared::util::spdlog_async_init spdlog_init;
		auto log{shared::util::create_logger("log")};

		curl_global_init(CURL_GLOBAL_ALL);	// TODO: call on demand
		
		collect();
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}