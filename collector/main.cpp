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
	std::unique_ptr<collector::source::feed> feed{collector::factory::feed(info.dest_)};
	feed->set_data_write(std::make_unique<keeper::data_write>(keeper_cfg, info.dest_.data_uri_));
	std::unique_ptr<collector::source::base> src{collector::factory::source(info.source_uri_, info.source_args_)};
	src->fetch_to(*feed);
}
//---------------------------------------------------------------------------------------------------------
void collect()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::metadata metadata{keeper_cfg};
	std::vector<keeper::metadata::source_info> const sources{metadata.load_source_meta()};
	for (auto const& source_info : sources)
	{
		if (!source_info.pending_)
		{
			continue;
		}
		try
		{
			SPDLOG_LOGGER_INFO(shared::util::log(), "processing source '{}' ({})...", source_info.source_uri_, source_info.source_args_);
			process_source(source_info, keeper_cfg);
			SPDLOG_LOGGER_INFO(shared::util::log(), "...done");
			metadata.drop_pending_flag(source_info.source_id_);
		}
		catch (std::exception const& ex)
		{
			SPDLOG_LOGGER_ERROR(shared::util::log(), "...error: {}", ex.what());
		}
	}
}
//---------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	shared::util::spdlog_async_init const spdlog_init;
	auto log{shared::util::create_logger()};
	SPDLOG_LOGGER_INFO(log, "collector started");

	try
	{
		curl_global_init(CURL_GLOBAL_ALL);	// TODO: call on demand
		
		collect();
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_CRITICAL(log, ex.what());
	}

	SPDLOG_LOGGER_INFO(log, "collector finished");

	return 0;
}