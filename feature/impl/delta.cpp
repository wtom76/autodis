#include "../pch.hpp"
#include "../shop.hpp"
#include "delta.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::delta::_evaluate(index_value_t idx_val)
{
	if (!bounds_.test(idx_val))
	{
		throw feature_out_of_bounds{idx_val};
	}
	return
		underlying_.second->value(shop_.index().next(idx_val, shift_.second)) -
		underlying_.first->value(shop_.index().next(idx_val, shift_.first));
}
//---------------------------------------------------------------------------------------------------------
feature::impl::delta::delta(nlohmann::json cfg, shop& shop)
	: abstract{std::move(cfg)}
	, shop_{shop}
	, shift_{cfg_.at("shift_first").get<std::ptrdiff_t>(), cfg_.at("shift_second").get<std::ptrdiff_t>()}
	, underlying_{shop_.feature(cfg.at("underlying_first")), shop_.feature(cfg.at("underlying_second"))}
{
	if (shift_.first == 0 || shift_.second == 0)
	{
		throw std::runtime_error("0 shift doesn't make sence. can only be negative"s);
	}
	if (shift_.first > 0 || shift_.second > 0)
	{
		throw std::runtime_error("shift can only be negative - peeking into the future is forbidden for nontarget entities"s);
	}
	std::pair<abstract::bounds_data, abstract::bounds_data> bounds{underlying_.first->bounds(), underlying_.second->bounds()};
	bounds.first.index_min_ = shop_.index().next(bounds.first.index_min_, shift_.first);
	bounds.second.index_min_ = shop_.index().next(bounds.second.index_min_, shift_.second);
	bounds.first.index_min_ = std::max(bounds.first.index_min_, bounds.second.index_min_);
	bounds.first.value_min_ = bounds.second.value_min_ - bounds.first.value_max_;
	bounds.first.value_max_ = bounds.second.value_max_ - bounds.first.value_min_;
	_set_bounds(bounds.first);
}
