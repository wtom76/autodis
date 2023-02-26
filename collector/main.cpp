#include "pch.hpp"
#include <keeper/keeper.hpp>
#include <shared/shared.hpp>
#include "source/file.hpp"
#include "factory.hpp"

// TODO:
// create keeper data
// pass dest_uri to keeper data
// pass keeper data to feed
// run fetch_to

int test()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::metadata metadata{keeper_cfg};
	const auto series_info{metadata.load()};
	for (auto const& info : series_info)
	{
		auto feed{collector::factory::feed(info.feed_uri_)};
		auto src{collector::factory::source(info.source_uri_)};
		feed->start(std::make_unique<keeper::data>(keeper_cfg, info.data_uri_));
		src->fetch_to(*feed);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	try
	{
		return test();
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}