#include "pch.hpp"
#include "learn_runner.hpp"

//----------------------------------------------------------------------------------------------------------
shared::data::frame test_load_data(std::vector<keeper::data_uri> const& uris)
{
	shared::data::frame df;
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};
	dr.read(uris, df);
	df.print_head(std::cout);
	return df;
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
double test(std::vector<std::size_t> const& layers_sizes)
{
	shared::data::frame df{test_load_data(
		{
			"test_linear/x"s,
			"test_linear/y"s
		}
	)};
	test_normalize(df);
	shared::data::view dw{df};

	learning::config mfn_cfg{layers_sizes};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::rprop<learning::multilayer_feed_forward> teacher{
		std::make_pair(df, dw),
		 {"x"s},
		 {"y"s}};

	autodis::learn_runner<learning::multilayer_feed_forward> runner{mfn_cfg, mfn, teacher};
	runner.wait();
	return runner.min_err();
}

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		double min_msqrerr{std::numeric_limits<double>::max()};
		std::size_t best_n{0};
		for (std::size_t n{1}; n <= 20; ++n)
		{
			for (int i{10}; i; --i)
			{
				const double err{test({1, n, 1})};
				if (err < min_msqrerr)
				{
					min_msqrerr = err;
					best_n = n;
				}
			}
		}
		std::cout << "test ended\nbest internal layer size: " << best_n << "\nmean square error of best result: " << min_msqrerr << std::endl;
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}