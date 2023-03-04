#include "pch.h"
#include "common.hpp"
#include <shared/shared.hpp>
#include <keeper/keeper.hpp>
#include <learning/learning.hpp>

void test()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	std::vector<keeper::data_uri> uris{
		"000001/f1"s,
		"000001/f2"s,
		"000001/f3"s,
		"000001/f4"s,
		"000001/f5"s
	};
	shared::data::frame df;
	dr.read(uris, df);

	std::cout << "index";
	for (std::size_t i{0}; i != df.col_count(); ++i)
	{
		std::cout << ';' << df.names()[i];
	}
	std::cout << "\n" << df.index()[0];
	for (std::size_t i{0}; i != df.col_count(); ++i)
	{
		std::cout << ';' << df.series(i)[0];
	}
	std::cout << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		test();
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}