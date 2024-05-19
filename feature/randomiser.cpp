#include "pch.hpp"
#include "randomiser.hpp"

//---------------------------------------------------------------------------------------------------------
feature::randomiser::randomiser()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();
	std::vector<keeper::metadata::feature_info> feature_info{keeper::metadata{keeper_cfg}.load_feature_meta_all()};
	for (keeper::metadata::feature_info const& info : feature_info)
	{
		type_map_[info.type_id_].emplace_back(info);
	}
}
//---------------------------------------------------------------------------------------------------------
void feature::randomiser::_verify_typeset(std::vector<std::int32_t> const& type_ids, std::string const& label)
{
	if (type_ids.empty())
	{
		throw std::runtime_error("underlying_..._types is empty in "s + label);
	}
	for (std::int32_t id : type_ids)
	{
		if (!type_map_.contains(id))
		{
			throw std::runtime_error("unknown underlying_types: "s + std::to_string(id));
		}
	}
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::randomiser::_spec_delta(feature_info_t const& feature_template)
{
	feature::feature_info_t result;
	[[maybe_unused]] std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>> type_ids;
	feature_template.formula_.at("underlying_1_types"sv).get_to(type_ids.first);
	feature_template.formula_.at("underlying_2_types"sv).get_to(type_ids.second);

	_verify_typeset(type_ids.first, feature_template.label_);
	_verify_typeset(type_ids.second, feature_template.label_);

	std::int32_t const type_1{_pick_random(type_ids.first)};
	std::int32_t const type_2{_pick_random(type_ids.second)};

	result.formula_["type"s] = "delta"s;
	result.formula_["underlying_1"s] = _pick_random(type_map_[type_1]).id_;
	result.formula_["underlying_2"s] = _pick_random(type_map_[type_2]).id_;

	return result;
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::randomiser::specialise(feature_info_t const& feature_template)
{
	std::string_view const template_type{feature_template.formula_.at("template_type"sv).get<std::string_view>()};
	if (template_type == "delta"sv)
	{
		return _spec_delta(feature_template);
	}
	else if (template_type == "shift_delta"sv)
	{}
	throw std::runtime_error("unknown template type: "s + feature_template.label_);
}
