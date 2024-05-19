#include "../pch.hpp"
#include "../shop.hpp"
#include "../index_cursor.hpp"
#include "shift_delta.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
void feature::impl::shift_delta::_init()
{
	bounds_data bounds;
	index_cursor c1{_mi(), *underlying_.first, underlying_.first->bounds().index_min_};
	c1.prev(shift_.first);
	index_cursor c2{_mi(), *underlying_.second, underlying_.second->bounds().index_min_};
	c2.prev(shift_.second);
	bounds.index_min_ = std::max(c1.current(), c2.current());

	index_pos_t master_pos{_mi().pos(bounds.index_min_)};
	c1.position(bounds.index_min_);
	c2.position(bounds.index_min_);
	if (static_cast<std::size_t>(master_pos) >= _mi().size() ||
		shift_.first != c1.next(shift_.first) ||
		shift_.second != c2.next(shift_.second))
	{
		throw not_enough_data(label());
	}
	do
	{
		data_.emplace(
			_mi().at(master_pos++),
			underlying_.second->value(c1.current()) -
			underlying_.first->value(c2.current()));
	}
	while (static_cast<std::size_t>(master_pos) != _mi().size() && c1.next() && c2.next());

	bounds.index_max_ = _mi().at(master_pos - 1);
	bounds.value_min_ = underlying_.second->bounds().value_min_ - underlying_.first->bounds().value_max_;
	bounds.value_max_ = underlying_.second->bounds().value_max_ - underlying_.first->bounds().value_min_;

	_set_bounds(bounds);
}
//---------------------------------------------------------------------------------------------------------
feature::impl::shift_delta::shift_delta(feature_info_t&& info, shop& shop)
	: abstract{std::move(info), shop}
	, shift_{cfg().at("shift_1").get<std::ptrdiff_t>(), cfg().at("shift_2").get<std::ptrdiff_t>()}
	, underlying_{shop_.feature(cfg().at("underlying_1")), shop_.feature(cfg().at("underlying_2"))}
{
	_init();
}
