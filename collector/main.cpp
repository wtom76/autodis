#include "pch.h"
#include "source/file.h"
#include "feed/finam_daily_file.h"
#include <keeper/keeper.h>
#include <shared/shared.h>

int test(int argc, char* argv[])
{
	if (argc != 2)
	{
		return 0;
	}
	{
		keeper::config keeper_cfg;
		keeper_cfg.load();
		keeper::metadata metadata(keeper_cfg);
		const auto series_info{metadata.load_all_series_info()};
		for (auto const& info : series_info)
		{
			shared::util::uri feed_uri{info.feed_uri_};
			for (std::string const& part : feed_uri.parts())
			{
				std::cout << part << '\t';
			}
			std::cout << '\n';
		}
	}
	collector::source::file src{argv[1]};
	collector::feed::finam_daily_file dest;
	src.fetch_to(dest);
	return 0;
}

int main(int argc, char* argv[])
{
	try
	{
		return test(argc, argv);
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}