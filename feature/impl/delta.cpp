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
	, underlying_{shop_.feature(cfg_.at("underlying_first")), shop_.feature(cfg_.at("underlying_second"))}
{
	std::pair<abstract::bounds_data, abstract::bounds_data> bounds{underlying_.first->bounds(), underlying_.second->bounds()};

	bounds.first.index_min_ = shop_.index().safe_next(bounds.first.index_min_, -shift_.first);
	bounds.second.index_min_ = shop_.index().safe_next(bounds.second.index_min_, -shift_.second);
	bounds.first.index_min_ =
		std::min(shop_.index().max(),
			std::max(shop_.index().min(),
				std::max(bounds.first.index_min_, bounds.second.index_min_)));

	bounds.first.index_max_ = shop_.index().safe_next(bounds.first.index_max_, -shift_.first);
	bounds.second.index_max_ = shop_.index().safe_next(bounds.second.index_max_, -shift_.second);
	bounds.first.index_max_ =
		std::min(shop_.index().max(),
			std::max(shop_.index().min(),
				std::min(bounds.first.index_max_, bounds.second.index_max_)));

	bounds.first.value_min_ = bounds.second.value_min_ - bounds.first.value_max_;
	bounds.first.value_max_ = bounds.second.value_max_ - bounds.first.value_min_;

	_set_bounds(bounds.first);
}
