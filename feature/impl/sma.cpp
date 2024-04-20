#include "../pch.hpp"
#include "../shop.hpp"
#include "sma.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::sma::_calc_from_scratch(index_value_t idx_val) const
{
	value_t sum{0.};
	master_index::index_pos_t master_pos{shop_.index().pos(idx_val)};
	for (std::ptrdiff_t i{0}; i != period_; ++i)
	{
		value_t const val{underlying_->value(shop_.index().val(master_pos--))};
		if (std::isnan(val))
		{
			return shared::data::nan;
		}
		sum += val;
	}
	return sum / period_;
}
//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::sma::_calc(index_value_t idx_val) const
{
	if (idx_val == bounds_.index_min_)
	{
		return _calc_from_scratch(idx_val);
	}
	auto const data_i{data_.find(shop_.index().next(idx_val, -1))};
	if (data_i == data_.cend() || std::isnan(data_i->second))
	{
		return _calc_from_scratch(idx_val);
	}
	return data_i->second + (underlying_->value(idx_val) - underlying_->value(shop_.index().next(idx_val, -period_))) / period_;
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
	bounds.index_min_ = shop_.index().next(bounds.index_min_, period_);
	_set_bounds(bounds);
}
