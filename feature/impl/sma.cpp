#include "../pch.hpp"
#include "../shop.hpp"
#include "sma.hpp"
#include "error.hpp"
#include "util.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::sma::_calc_from_scratch(index_value_t idx_val) const
{
	assert(period_ > 0);
	value_t sum{0.};
	master_index::index_pos_t master_pos{shop_.index().pos(idx_val)};
	for (std::ptrdiff_t i{period_}; i;)
	{
		value_t const val{underlying_->value(shop_.index().at(master_pos--))};
		if (!std::isnan(val))
		{
			sum += val;
			--i;
		}
	}
	return sum / period_;
}
//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::sma::_under_val_out(index_value_t idx_val) const
{
	auto const next_result{solid_next(shop_.index(), *underlying_, idx_val, -period_)};
	if (-next_result.second < period_)
	{
		throw not_enough_data{label()};
	}
	return underlying_->value(next_result.first);
}
//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::sma::_calc(index_value_t idx_val) const
{
	if (idx_val == bounds_.index_min_)
	{
		return _calc_from_scratch(idx_val);
	}
	auto const prev_i{data_.find(shop_.index().next(idx_val, -1))};
	if (prev_i == data_.cend() || std::isnan(prev_i->second))
	{
		return _calc_from_scratch(idx_val);
	}
	value_t const under_val_out{_under_val_out(idx_val)};
	if (std::isnan(under_val_out))
	{
		return shared::data::nan;
	}
	value_t const under_val_new{underlying_->value(idx_val)};
	if (std::isnan(under_val_new))
	{
		return prev_i->second;
	}
	return prev_i->second + (under_val_new - under_val_out) / period_;
}
//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::sma::_evaluate(index_value_t idx_val)
{
	if (!bounds_.test(idx_val))
	{
		throw feature_out_of_bounds{idx_val};
	}
	return _calc(idx_val);
}
//---------------------------------------------------------------------------------------------------------
feature::impl::sma::sma(feature_info_t&& info, shop& shop)
	: abstract{std::move(info)}
	, shop_{shop}
	, period_{cfg().at("period").get<decltype(period_)>()}
	, underlying_{shop_.feature(cfg().at("underlying"))}
{
	if (period_ <= 0)
	{
		throw std::runtime_error("period should be positive"s);
	}
	abstract::bounds_data bounds{underlying_->bounds()};
	auto const next_result{solid_next(shop_.index(), *underlying_, bounds.index_min_, period_)};
	if (next_result.second < period_)
	{
		throw not_enough_data{label()};
	}
	bounds.index_min_ = next_result.first;
	_set_bounds(bounds);
}
