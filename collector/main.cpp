#include "pch.hpp"
#include <keeper/keeper.hpp>
#include <shared/shared.hpp>
#include "source/file.hpp"
#include "factory.hpp"

//---------------------------------------------------------------------------------------------------------
void collect(keeper::metadata::series_info const& info, keeper::config const& keeper_cfg)
{
	assert(info.pending_);
	auto feed{collector::factory::feed(info.feed_uri_)};
	auto src{collector::factory::source(info.source_uri_)};
	feed->start(std::make_unique<keeper::data_write>(keeper_cfg, info.data_uri_));
	src->fetch_to(*feed);
}
//---------------------------------------------------------------------------------------------------------
void collect()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::metadata metadata{keeper_cfg};
	const auto series_info{metadata.load()};
	for (auto const& info : series_info)
	{
		if (!info.pending_)
		{
			continue;
		}
		try
		{
			collect(info, keeper_cfg);
			metadata.drop_pending_flag(info.id_);
		}
		catch (std::exception const& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
}
//---------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		collect();
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}