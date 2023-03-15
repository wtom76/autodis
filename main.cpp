#include "pch.hpp"
#include <shared/shared.hpp>
#include <keeper/keeper.hpp>
#include <learning/learning.hpp>
#include "common.hpp"
#include "learn_runner.hpp"

//----------------------------------------------------------------------------------------------------------
void test_load_data(shared::data::frame& df)
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

//----------------------------------------------------------------------------------------------------------
void test()
{
	shared::data::frame df;
	test_load_data(df);
	shared::data::view dw{df};

	learning::config mfn_cfg{{5, 15, 1}};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::rprop<learning::multilayer_feed_forward> teacher{
		std::make_pair(df, dw),
		 {"f1"s, "f2"s, "f3"s, "f4"s, "f5"s},
		 {"f4"s}};

	autodis::learn_runner<learning::multilayer_feed_forward> runner{mfn_cfg, mfn, teacher};
}

//----------------------------------------------------------------------------------------------------------
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