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
	, period_{cfg().at("period"sv).get<decltype(period_)>()}
	, underlying_{shop_.feature(cfg().at("underlying"sv))}
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

	result.type_id_ = type_id_;
	result.formula_["type"s] = "sma"s;
	result.label_ = feature_template.label_;

	if (feature_template.formula_.contains("underlying_types"s))
	{
		std::vector<type_id_t> under_type_ids;
		feature_template.formula_.at("underlying_types"sv).get_to(under_type_ids);
		_verify_typeset(under_type_ids, feature_template.label_);
		result.formula_["underlying"s] = shop.random_info_of_type(shop.random_value(under_type_ids));
	}
	else if (feature_template.formula_.contains("underlying"s))
	{
		std::vector<id_t> under_ids;
		feature_template.formula_.at("underlying"sv).get_to(under_ids);
		result.formula_["underlying"s] = shop.random_info(under_ids);
	}

	{
		constexpr std::ptrdiff_t period_min{2};
		std::ptrdiff_t period_max{0};
		feature_template.formula_.at("period_max"sv).get_to(period_max);
		result.formula_["period"s] = shop.random_value(period_min, period_max);
	}

	return result;
}
