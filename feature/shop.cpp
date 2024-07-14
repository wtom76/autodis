#include "pch.hpp"
#include "shop.hpp"
#include "impl/stored.hpp"
#include "impl/delta.hpp"
#include "impl/shift_delta.hpp"
#include "impl/sma.hpp"

//---------------------------------------------------------------------------------------------------------
feature::shop::shop()
{
	keeper::config keeper_cfg;
	keeper_cfg.load();

	keeper_md_ = std::make_shared<keeper::metadata>(keeper_cfg);
	keeper_dr_ = std::make_shared<keeper::data_read>(keeper_cfg);
	master_index_.load(*keeper_dr_);

	std::vector<keeper::metadata::feature_info> feature_info{keeper::metadata{keeper_cfg}.load_feature_meta_all()};
	for (keeper::metadata::feature_info const& info : feature_info)
	{
		type_map_[info.type_id_].emplace_back(info);
	}
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::shop::_load_feature_info(std::int64_t feature_id) const
{
	std::vector<feature_info_t> feature_info{keeper_md_->load_feature_meta({feature_id})};
	if (feature_info.empty())
	{
		throw std::runtime_error("failed to load feature info by id: "s + std::to_string(feature_id));
	}
	assert(feature_info.front().id_ == feature_id);
	return std::move(feature_info.front());
}
//---------------------------------------------------------------------------------------------------------
std::string_view feature::shop::_feature_type(feature_info_t const& info)
{
	return info.formula_.at("type"sv).get<std::string_view>();
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::shop::_rnd_from_template(feature_info_t const& feature_template)
{
	std::string_view const template_type{feature_template.formula_.at("template_type"sv).get<std::string_view>()};
	if (template_type == "stored"sv)
	{
		return impl::stored::rnd_from_template(feature_template, *this);
	}
	if (template_type == "delta"sv)
	{
		return impl::delta::rnd_from_template(feature_template, *this);
	}
	if (template_type == "shift_delta"sv)
	{
		return impl::shift_delta::rnd_from_template(feature_template, *this);
	}
	if (template_type == "sma"sv)
	{
		return impl::sma::rnd_from_template(feature_template, *this);
	}
	throw std::runtime_error("unknown template type: "s + feature_template.label_);
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_create_feature(feature_info_t&& info)
{
	std::string_view feature_type{_feature_type(info)};

	if (feature_type == "template"sv)
	{
		return _create_feature(_rnd_from_template(std::move(info)));
	}
	std::shared_ptr<feature::abstract> result;
	if (feature_type == "stored"sv)
	{
		result = std::make_shared<impl::stored>(std::move(info), *this, keeper_dr_);
	}
	else if (feature_type == "delta"sv)
	{
		result = std::make_shared<impl::delta>(std::move(info), *this);
	}
	else if (feature_type == "shift_delta"sv)
	{
		result = std::make_shared<impl::shift_delta>(std::move(info), *this);
	}
	else if (feature_type == "sma"sv)
	{
		result = std::make_shared<impl::sma>(std::move(info), *this);
	}
	else
	{
		throw std::runtime_error("unknown feature type. feature id: "s + std::to_string(info.id_));
	}
	assert(result);
	feature_map_.emplace(info.id_, result);
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_existing(std::int64_t feature_id) const
{
	auto const feature_map_i{feature_map_.find(feature_id)};
	return feature_map_i != feature_map_.cend()
			? feature_map_i->second
			: std::shared_ptr<feature::abstract>{};
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_feature(std::int64_t feature_id)
{
	if (std::shared_ptr<abstract> existing{_existing(feature_id)})
	{
		assert(_feature_type(existing->info()) != "template"sv);
		return existing;
	}
	try
	{
		return _create_feature(_load_feature_info(feature_id));
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature with id: {}. error: {}", feature_id, ex.what());
		throw;
	}
}
//---------------------------------------------------------------------------------------------------------
// 1. get already existing in shop feature by id or create it using formula from DB otherwise
// 2. or create feature from json representing feature_info_t

// TODO: shop doesn't replace json
// TODO: if feature_info_t has non template id, no need to store it as feature_info_t. store id.
// TODO: thus store feature_info_t instead of id into json if id == 0
std::shared_ptr<feature::abstract> feature::shop::_feature(nlohmann::json const& fj)
{
	// 1.
	if (fj.is_number())
	{
		return _feature(fj.get<std::int64_t>());
	}
	// 2.
	if (fj.is_object())
	{
		feature_info_t info;
		fj.get_to(info);
		if (_feature_type(info) == "template"sv)
		{
			throw std::runtime_error{"json representation of feature_info should not be template. for templates use DB id instead"};
		}
		if (info.id_)
		{
			SPDLOG_LOGGER_WARN(log(), "store id instead of json representation of feature_info when id is not 0");
		}
		return _create_feature(std::move(info));
	}
	throw std::runtime_error{"invalid json representation of feature_info"};
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::feature(nlohmann::json const& fj)
{
	try
	{
		return _feature(fj);
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature from {}", fj.dump());
		throw;
	}
}
//---------------------------------------------------------------------------------------------------------
void feature::shop::verify_typeset(std::vector<std::int32_t> const& type_ids, std::string const& label)
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
feature::feature_info_t const& feature::shop::random_feature_info(std::int32_t feature_type_id)
{
	return randomiser_.pick_random(type_map_[feature_type_id]);
}
