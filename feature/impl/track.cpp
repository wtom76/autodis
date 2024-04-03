#include "../pch.hpp"
#include "track.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::track::_evaluate(index_value_t /*idx_val*/)
{
	return 0.;
}
//---------------------------------------------------------------------------------------------------------
feature::impl::track::track(nlohmann::json cfg, shop& shop)
	: abstract{std::move(cfg)}
	, depth_{cfg_.at("depth").get<decltype(depth_)>()}
	, underlying_{shop.feature(cfg.at("underlying"))}
{
//	shop.feature(cfg.at("underlying"))->bounds()
}
