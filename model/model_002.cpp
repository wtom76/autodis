#include "pch.hpp"
#include "model_002.hpp"
#include "learn_runner.hpp"
#include <shared/math/target_delta.hpp>
#include <shared/math/track.hpp>

//---------------------------------------------------------------------------------------------------------
// load initial (raw) data from DB
void autodis::model::model_002::_load_data()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	keeper::data_read dr{keeper_cfg};

	{
		std::vector<keeper::data_uri> const uris
		{
			"000001/f4"s,			// GAZP close
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "GAZP_close"s;
		df_.outer_join(std::move(df));
	}
	{
		std::vector<keeper::data_uri> const uris
		{
			"000002/f4"s			// GOLD close
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "GOLD_close"s;
		df_.outer_join(std::move(df));
	}
	{
		std::vector<keeper::data_uri> const uris
		{
			"000003/f4"s			// IMOEX close
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "IMOEX_close"s;
		df_.outer_join(std::move(df));
	}
	original_series_count_ = df_.col_count();
	assert(original_series_count_ == 3);

	{
		std::vector<keeper::data_uri> const uris
		{
			"000002/f1"s			// GOLD open
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "GOLD_close"s;
		df_vis_.outer_join(std::move(df));
	}
	{
		std::vector<keeper::data_uri> const uris
		{
			"000002/f2"s			// GOLD high
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "GOLD_close"s;
		df_vis_.outer_join(std::move(df));
	}
	{
		std::vector<keeper::data_uri> const uris
		{
			"000002/f3"s			// GOLD low
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "GOLD_close"s;
		df_vis_.outer_join(std::move(df));
	}
	{
		std::vector<keeper::data_uri> const uris
		{
			"000002/f4"s			// GOLD close
		};
		shared::data::frame df;
		dr.read(uris, df);
		assert(std::ranges::is_sorted(df.index()));
		df.name(0) = "GOLD_close"s;
		df_vis_.outer_join(std::move(df));
	}
}
//---------------------------------------------------------------------------------------------------------
// add computed target to df_
void autodis::model::model_002::_create_target()
{
	shared::math::target_delta(df_, 0, df_);
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_create_features()
{
	constexpr std::size_t track_depth{5};
	for (size_t i{0}; i != original_series_count_; ++i)
	{
		shared::math::track(df_, i, track_depth);
	}
}
//---------------------------------------------------------------------------------------------------------
// - 1. delete original closes
// 2. delete rows contaning nans
void autodis::model::model_002::_clear_data()
{
	// 1.
	//for (size_t i{0}; i != original_series_count_; ++i)
	//{
	//	df_.delete_series(0);	// always 0. 1 will be 0 after first delete
	//}
	// 2.
	df_ = df_.clear_lacunas(); // normalize should be called on lacune free data, so should clear frame not view till normalize is applied to the former
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_normalize()
{
	norm_.clear();
	norm_.reserve(df_.col_count());
	for (std::size_t i{0}; i != df_.col_count(); ++i)
	{
		norm_.emplace_back(shared::math::tanh_normalization{});
		norm_.back().normalize(df_.series(i));
	}
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::model_002::_learn()
{
	std::vector<std::size_t> const layers_sizes{15, 40, 1};
	shared::data::view dw{df_};

	learning::config mfn_cfg{layers_sizes};
	learning::multilayer_feed_forward mfn{mfn_cfg};
	learning::rprop<learning::multilayer_feed_forward> teacher{
		std::make_pair(df_, dw),
		{
			"GAZP_close_delta(t-1)"s,
			"GAZP_close_delta(t-2)"s,
			"GAZP_close_delta(t-3)"s,
			"GAZP_close_delta(t-4)"s,
			"GAZP_close_delta(t-5)"s,
			"GOLD_close_delta(t-1)"s,
			"GOLD_close_delta(t-2)"s,
			"GOLD_close_delta(t-3)"s,
			"GOLD_close_delta(t-4)"s,
			"GOLD_close_delta(t-5)"s,
			"IMOEX_close_delta(t-1)"s,
			"IMOEX_close_delta(t-2)"s,
			"IMOEX_close_delta(t-3)"s,
			"IMOEX_close_delta(t-4)"s,
			"IMOEX_close_delta(t-5)"s
		},
		{
			"GAZP_close_delta(t+1)"s
		}
	};

	autodis::learn_runner<learning::multilayer_feed_forward> runner{mfn_cfg, mfn, teacher};
	runner.wait();
	min_err_ = runner.min_err();
}
//---------------------------------------------------------------------------------------------------------
// learn
void autodis::model::model_002::run()
{
	_load_data();

	_create_target();
	_create_features();

	{
		df_.print_shape(std::cout);
		std::cout << "\n\n";
		std::ofstream f{"df.csv"s};
		df_.print(f);
	}

	_clear_data();
	_normalize();

	autodis::visual::chart chrt{df_vis_};
	chrt.add_candlesticks({0, 1, 2, 3});
	chrt.add_line(3);
	chrt.show();

	{
		std::ofstream f{"df_norm.csv"s};
		df_.print(f);
	}

	//_learn();
}