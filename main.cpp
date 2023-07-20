#include "pch.hpp"
#include "learn_runner.hpp"
#include "model/model_000.hpp"
#include "model/model_001.hpp"
#include "model/model_002.hpp"

#include "visual/chart.hpp"

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
		dw,
		{"x"s},
		{"y"s}
	};

	autodis::learn_runner<learning::multilayer_feed_forward> runner{mfn_cfg, mfn, teacher};
	runner.wait();
	return runner.min_err();
}

//----------------------------------------------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	try
	{
		shared::util::spdlog_async_init spdlog_init;
		auto log{shared::util::create_console_logger()};

		autodis::model::model_002{}.run();
	}
	catch (std::exception const& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}