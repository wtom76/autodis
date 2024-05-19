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
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_create_feature(std::int64_t feature_id)
{
	std::vector<keeper::metadata::feature_info> feature_info{keeper_md_->load_feature_meta({feature_id})};
	if (feature_info.empty())
	{
		throw std::runtime_error("failed to load feature info by id: "s + std::to_string(feature_id));
	}
	assert(feature_info.front().id_ == feature_id);
	return _create_feature(std::move(feature_info.front()));
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_create_feature(keeper::metadata::feature_info&& info)
{
	std::string_view feature_type;
	info.formula_.at("type"sv).get_to(feature_type);

	if (feature_type == "template"sv)
	{
		return _create_feature(randomiser_.specialise(std::move(info)));
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
std::shared_ptr<feature::abstract> feature::shop::_feature(nlohmann::json& fj)
{
	constexpr std::string_view generated_from_template_tag{"generated"sv};
	if (fj.is_number())
	{
		return feature(fj.get<std::int64_t>());
	}
	if (fj.is_object())
	{
		auto const cached_i{fj.find(generated_from_template_tag)};		// is used to cache specialised templates only
		if (cached_i != fj.end())
		{
			keeper::metadata::feature_info info{cached_i->get<keeper::metadata::feature_info>()};
			return _create_feature(std::move(info));
		}
		auto const id_i{fj.find("id"sv)};								// fall back to id
		if (id_i != fj.end())
		{
			std::shared_ptr<feature::abstract> feature{_create_feature(id_i->get<std::int64_t>())};
			fj[generated_from_template_tag] = feature->info();
			return feature;
		}
	}
	throw std::runtime_error{"invalid json representation of feature_info"};
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::feature(std::int64_t feature_id)
{
	auto const feature_map_i{feature_map_.find(feature_id)};
	if (feature_map_i != feature_map_.cend())
	{
		return feature_map_i->second;
	}
	try
	{
		return _create_feature(feature_id);
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature with id: {}. error: {}", feature_id, ex.what());
		throw;
	}
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::feature(nlohmann::json& fj)
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
