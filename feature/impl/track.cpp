#include "../pch.hpp"
#include "../shop.hpp"
#include "track.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::track::_evaluate(index_value_t idx_val)
{
	if (!bounds_.test(idx_val))
	{
		throw std::runtime_error("an index out of bounds"s);
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
	abstract::bounds_data bounds{underlying_->bounds()};
	bounds.index_min_ = shop_.index().next(bounds.index_min_, depth_);
	_set_bounds(bounds);
}
