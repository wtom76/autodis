#include "../pch.hpp"
#include "../shop.hpp"
#include "../index_cursor.hpp"
#include "sma.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
void feature::impl::sma::_init()
{
	assert(period_ > 1);

	value_t avg{0.};
	index_cursor c_in{_mi(), *underlying_, underlying_->bounds().index_min_};
	for (std::ptrdiff_t count_left{period_}, n{1}; count_left && n; --count_left, n = c_in.next())
	{
		value_t const val{underlying_->value(c_in.current())};
		assert(!std::isnan(val));
		avg += val;
	}
	c_in.prev();
	bounds_data bounds{underlying_->bounds()};
	bounds.index_min_ = c_in.current();
	avg /= period_;
	data_.emplace(c_in.current(), avg);

	index_cursor c_out{_mi(), *underlying_, underlying_->bounds().index_min_};
	while (c_in.next())
	{
		data_.emplace(
			c_in.current(),
			avg + (underlying_->value(c_in.current()) - underlying_->value(c_out.current())) / period_
		);
		c_out.next();
	}

	bounds.index_max_ = c_in.current();
	_set_bounds(bounds);
}
//---------------------------------------------------------------------------------------------------------
feature::impl::sma::sma(feature_info_t&& info, shop& shop)
	: abstract{std::move(info), shop}
	, period_{cfg().at("period").get<decltype(period_)>()}
	, underlying_{shop_.feature(cfg().at("underlying"))}
{
	if (period_ <= 1)
	{
		throw std::runtime_error("period should be more than 1"s);
	}
	_init();
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::impl::sma::rnd_from_template(feature_info_t const& feature_template, shop& shop)
{
	feature::feature_info_t result;

	result.formula_["type"s] = "sma"s;

	{
		std::int64_t under_id;
		std::vector<std::int32_t> type_ids;
		feature_template.formula_.at("underlying_types"sv).get_to(type_ids);
		shop.verify_typeset(type_ids, feature_template.label_);
		under_id = shop.random_feature_info(shop.pick_random(type_ids)).id_;
		result.formula_["underlying"s] = under_id;
	}
	{
		constexpr std::ptrdiff_t period_min{2};
		std::ptrdiff_t period_max;
		feature_template.formula_.at("period_max"sv).get_to(period_max);
		result.formula_["pariod"s] = shop.pick_random(period_min, period_max);
	}

	return result;
}
