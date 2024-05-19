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
//---------------------------------------------------------------------------------------------------------
// 1. in special case of same underlying
//		prohibit same shifts
//		make first shift be always less than second shift
feature::feature_info_t feature::impl::shift_delta::rnd_from_template(feature_info_t const& feature_template, shop& shop)
{
	feature::feature_info_t result;

	result.formula_["type"s] = "shift_delta"s;

	std::pair<std::int64_t, std::int64_t> under_id;
	{
		std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>> type_ids;
		feature_template.formula_.at("underlying_1_types"sv).get_to(type_ids.first);
		feature_template.formula_.at("underlying_2_types"sv).get_to(type_ids.second);
		shop.verify_typeset(type_ids.first, feature_template.label_);
		shop.verify_typeset(type_ids.second, feature_template.label_);
		under_id.first = shop.random_feature_info(shop.pick_random(type_ids.first)).id_;
		under_id.second = shop.random_feature_info(shop.pick_random(type_ids.second)).id_;
		result.formula_["underlying_1"s] = under_id.first;
		result.formula_["underlying_2"s] = under_id.second;
	}
	{
		constexpr std::pair<std::ptrdiff_t, std::ptrdiff_t> shift_min{0, 0};
		std::pair<std::ptrdiff_t, std::ptrdiff_t> shift_max;
		feature_template.formula_.at("shift_1_max"sv).get_to(shift_max.first);
		feature_template.formula_.at("shift_2_max"sv).get_to(shift_max.second);
		std::pair<std::ptrdiff_t, std::ptrdiff_t> shift;
		shift.first = shop.pick_random(shift_min.first, shift_max.first);
		shift.second = shop.pick_random(shift_min.second, shift_max.first);

		// 1.
		if (under_id.first == under_id.second)
		{
			if (shift_max.first == shift_min.first && shift_max.second == shift_min.second)
			{
				throw std::runtime_error("shift max can't both be equivalent to min in "s + feature_template.label_);
			}
			int tries_count{1000};
			while (shift.first == shift.second)
			{
				shift.first = shop.pick_random(shift_min.first, shift_max.first);
				shift.second = shop.pick_random(shift_min.second, shift_max.first);
				if (!--tries_count)
				{
					throw std::runtime_error("long/infinite loop protection is hit when picking random shifts in "s + feature_template.label_);
				}
			}
			if (shift.first > shift.second)
			{
				std::swap(shift.first, shift.second);
			}
		}
		
		result.formula_["shift_1"s] = shift.first;
		result.formula_["shift_2"s] = shift.second;
	}

	return result;
}
