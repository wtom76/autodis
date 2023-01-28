#include "pch.h"
#include "source/file.h"
#include "feed/finam_daily_file.h"
#include <keeper/keeper.h>

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