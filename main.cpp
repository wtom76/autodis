#include "pch.hpp"
#include "learn_runner.hpp"

//----------------------------------------------------------------------------------------------------------
void test_load_data(shared::data::frame& df)
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	std::vector<keeper::data_uri> uris{
		"test_linear/x"s,
		"test_linear/y"s
	};
	dr.read(uris, df);
	df.print_head(std::cout);
}

//----------------------------------------------------------------------------------------------------------
void test_normalize(shared::data::frame& df)
{
	shared::math::range_normalization norm{{0, 1}};
	for (auto& series : df.data())
	{
		norm.normalize(series);
	}
}

//----------------------------------------------------------------------------------------------------------
void test()
{
	shared::data::frame df;
	test_load_data(df);
	test_normalize(df);
	shared::data::view dw{df};

	learning::config mfn_cfg{{5, 15, 1}};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::rprop<learning::multilayer_feed_forward> teacher{
		std::make_pair(df, dw),
		 {"x"s},
		 {"y"s}};

	autodis::learn_runner<learning::multilayer_feed_forward> runner{mfn_cfg, mfn, teacher};
}

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		test();
		std::cout << "test ended" << std::endl;
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}