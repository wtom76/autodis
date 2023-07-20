#include "pch.hpp"
#include "model_000.hpp"
#include "learn_runner.hpp"

//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::model_000::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};

	{
		std::vector<keeper::data_uri> const uris
		{
			"test_linear/x"s,
			"test_linear/y"s
		};
		dr.read(uris, df_);
		assert(std::ranges::is_sorted(df_.index()));
		df_.name(0) = "x"s;
		df_.name(1) = "y"s;
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_000::_normalize()
{
	norm_.clear();
	norm_.reserve(df_.col_count());
	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		norm_.emplace_back(shared::math::range_normalization{{-1, 1}});
		norm_.back().normalize(df_.series(i));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_000::_learn()
{
	std::vector<std::size_t> const layers_sizes{1, 4, 1};
	shared::data::view dw{df_};

	learning::config mfn_cfg{layers_sizes};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::rprop<learning::multilayer_feed_forward> teacher{
		dw,
		{
			"x"s
		},
		{
			"y"s
		}
	};

	autodis::learn_runner<learning::multilayer_feed_forward> runner{mfn_cfg, mfn, teacher};
	runner.wait();
	min_err_ = runner.min_err();
}
//---------------------------------------------------------------------------------------------------------
// learn
void autodis::model::model_000::run()
{
	_load_data();

	{
		df_.print_shape(std::cout);
		std::cout << "\n\n";
		std::ofstream f{"df.csv"s};
		df_.print(f);
	}

	_normalize();

	{
		std::ofstream f{"df_norm.csv"s};
		df_.print(f);
	}

	_learn();
}