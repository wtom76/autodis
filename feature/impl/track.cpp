#include "../pch.hpp"
#include "../shop.hpp"
#include "track.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::track::_evaluate(index_value_t idx_val)
{
	if (!bounds_.test(idx_val))
	{
		throw feature_out_of_bounds{idx_val};
	}
	return underlying_->value(idx_val) - underlying_->value(shop_.index().next(idx_val, -depth_));	// the sign is opposite to sign of track from math
}
//---------------------------------------------------------------------------------------------------------
feature::impl::track::track(nlohmann::json cfg, shop& shop)
	: abstract{std::move(cfg)}
	, shop_{shop}
	, depth_{cfg_.at("depth").get<decltype(depth_)>()}
	, underlying_{shop_.feature(cfg.at("underlying"))}
{
	if (depth_ <= 0)
	{
		throw std::runtime_error("depth should be positive"s);
	}
	abstract::bounds_data bounds{underlying_->bounds()};
	bounds.index_min_ = shop_.index().next(bounds.index_min_, depth_);
	_set_bounds(bounds);
}
