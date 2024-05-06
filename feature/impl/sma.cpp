#include "../pch.hpp"
#include "../shop.hpp"
#include "sma.hpp"
#include "error.hpp"
#include "index_cursor.hpp"

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
	: abstract{std::move(info)}
	, shop_{shop}
	, period_{cfg().at("period").get<decltype(period_)>()}
	, underlying_{shop_.feature(cfg().at("underlying"))}
{
	if (period_ <= 1)
	{
		throw std::runtime_error("period should be more than 1"s);
	}
	_init();
}
