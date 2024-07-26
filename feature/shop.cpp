#include "pch.hpp"
#include "shop.hpp"
#include "impl/stored.hpp"
#include "impl/delta.hpp"
#include "impl/shift_delta.hpp"
#include "impl/sma.hpp"

//---------------------------------------------------------------------------------------------------------
feature::shop::shop()
	: shop{0}
{}
//---------------------------------------------------------------------------------------------------------
feature::shop::shop(std::int64_t feature_set_id)
	: feature_set_id_{feature_set_id}
{
	keeper::config keeper_cfg;
	keeper_cfg.load();

	keeper_md_ = std::make_shared<keeper::metadata>(keeper_cfg);
	keeper_dr_ = std::make_shared<keeper::data_read>(keeper_cfg);
	master_index_.load(*keeper_dr_);
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
	std::string_view feature_type{info.feature_type()};

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
	if (info.id_)
	{
		feature_map_.emplace(info.id_, result);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
// TODO: reuse existing features not only with DB ids
std::shared_ptr<feature::abstract> feature::shop::_existing(feature_info_t const& info) const
{
	if (!info.id_)
	{
		return {};
	}
	auto const feature_map_i{feature_map_.find(info.id_)};
	return feature_map_i != feature_map_.cend()
			? feature_map_i->second
			: std::shared_ptr<feature::abstract>{};
}
//---------------------------------------------------------------------------------------------------------
// 1. get already existing in shop feature
// 2. or create it using formula from DB
// 3. or create feature from json representing feature_info_t
std::shared_ptr<feature::abstract> feature::shop::_feature(nlohmann::json const& fj)
{
	SPDLOG_LOGGER_DEBUG(log(), "_feature()\n{}", fj.dump(4));
	feature_info_t info;
	fj.get_to(info);

	if (info.is_from_db() && info.is_template())
	{
		throw std::runtime_error{"json representation of feature_info should not be template. for templates use DB id instead"};
	}
	// 1.
	if (std::shared_ptr<abstract> existing{_existing(info)})
	{
		assert(!existing->info().is_template());
		return existing;
	}
	// 2.
	if (info.is_from_db())
	{
		info = _load_feature_info(info.id_); // TODO: take from loaded info registry instead
	}
	// 3.
	return _create_feature(std::move(info));
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
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature from {}. error: {}", fj.dump(), ex.what());
		throw;
	}
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::shop::random_info(std::vector<std::int64_t> const& feature_ids)
{
	if (feature_ids.empty())
	{
		throw std::runtime_error("feature ids to randomly pick from shouldn't be empty"s);
	}
	feature::feature_info_t info{_load_feature_info(randomiser_.pick_random(feature_ids))};
	return info.is_template() ? _rnd_from_template(info) : info;
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::shop::random_info_of_type(std::int32_t feature_type_id)
{
	std::vector<std::int64_t> feature_ids{keeper_md_->load_feature_ids_by_type(feature_set_id_, feature_type_id)};
	if (feature_ids.empty())
	{
		throw std::runtime_error("no features of type "s + std::to_string(feature_type_id));
	}
	return random_info(feature_ids);
}
