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
	, underlying_{shop_.feature(cfg().at("underlying_1")), shop_.feature(cfg().at("underlying_2"))}
{
	_init();
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::impl::delta::rnd_from_template(feature_info_t const& feature_template, shop& shop)
{
	feature::feature_info_t result;
	result.formula_["type"s] = "delta"s;

	{
		std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>> type_ids;
		feature_template.formula_.at("underlying_1_types"sv).get_to(type_ids.first);
		feature_template.formula_.at("underlying_2_types"sv).get_to(type_ids.second);
		shop.verify_typeset(type_ids.first, feature_template.label_);
		shop.verify_typeset(type_ids.second, feature_template.label_);
		result.formula_["underlying_1"s] = shop.random_feature_info(shop.pick_random(type_ids.first)).id_;
		result.formula_["underlying_2"s] = shop.random_feature_info(shop.pick_random(type_ids.second)).id_;
	}

	return result;
}
