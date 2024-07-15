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
feature::feature_info_t feature::impl::delta::_rnd_underlying(feature_info_t const& feature_template, shop& shop, char num)
{
	std::string tag_underlying_types{"underlying_*_types"};
	std::string tag_underlying{"underlying_*"};
	tag_underlying_types[11] = tag_underlying[11] = num;

	if (feature_template.formula_.contains(tag_underlying_types))
	{
		std::vector<type_id_t> under_type_ids;
		feature_template.formula_.at(tag_underlying_types).get_to(under_type_ids);
		_verify_typeset(under_type_ids, feature_template.label_);
		return shop.random_info_of_type(shop.random_value(under_type_ids));
	}
	else if (feature_template.formula_.contains(tag_underlying))
	{
		std::vector<id_t> under_ids;
		feature_template.formula_.at(tag_underlying).get_to(under_ids);
		return shop.random_info(under_ids);
	}
	else
	{
		throw std::runtime_error{"neither "s + tag_underlying_types + " nor "s + tag_underlying + " provided"s};
	}
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
feature::impl::delta::delta(feature_info_t&& info, shop& shop)
	: abstract{std::move(info), shop}
	, underlying_{shop_.feature(cfg().at("underlying_1"sv)), shop_.feature(cfg().at("underlying_2"sv))}
{
	_init();
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::impl::delta::rnd_from_template(feature_info_t const& feature_template, shop& shop)
{
	feature::feature_info_t result;
	result.type_id_ = type_id_;
	result.formula_["type"s] = "delta"s;
	result.label_ = feature_template.label_;
	result.formula_["underlying_1"s] = _rnd_underlying(feature_template, shop, '1');
	result.formula_["underlying_2"s] = _rnd_underlying(feature_template, shop, '2');
	return result;
}
