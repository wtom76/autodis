#include "../pch.hpp"
#include "../shop.hpp"
#include "../index_cursor.hpp"
#include "delta.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
void feature::impl::delta::_init()
{
	bounds_data bounds;
	bounds.index_min_ = std::max(underlying_.first->bounds().index_min_, underlying_.second->bounds().index_min_);
	bounds.index_max_ = std::min(underlying_.first->bounds().index_max_, underlying_.second->bounds().index_max_);

	index_pos_t master_pos{_mi().pos(bounds.index_min_)};
	index_pos_t const master_last{_mi().pos(bounds.index_max_)};

	// skip lacunes
	for (; master_pos <= master_last; ++master_pos)
	{
		index_value_t const ival{_mi().at(master_pos)};
		if (!std::isnan(underlying_.first->value(ival)) && !std::isnan(underlying_.second->value(ival)))
		{
			break;
		}
	}
	// fill data
	for (; master_pos <= master_last; ++master_pos)
	{
		index_value_t const ival{_mi().at(master_pos)};
		data_.emplace(ival, underlying_.second->value(ival) - underlying_.first->value(ival));
	}

	if (data_.empty())
	{
		throw not_enough_data(label());
	}

	bounds.value_min_ = underlying_.second->bounds().value_min_ - underlying_.first->bounds().value_max_;
	bounds.value_max_ = underlying_.second->bounds().value_max_ - underlying_.first->bounds().value_min_;

	_set_bounds(bounds);
}
//---------------------------------------------------------------------------------------------------------
feature::impl::delta::delta(feature_info_t&& info, shop& shop)
	: abstract{std::move(info), shop}
	, underlying_{shop_.feature(cfg().at("underlying_first")), shop_.feature(cfg().at("underlying_second"))}
{
	_init();
}
