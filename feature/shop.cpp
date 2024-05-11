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
	return _create_feature(std::move(feature_info.front()));
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_create_feature(keeper::metadata::feature_info&& info)
{
	std::string_view feature_type;
	info.formula_.at("type"sv).get_to(feature_type);

	if (feature_type == "stored"sv)
	{
		return std::make_shared<impl::stored>(std::move(info), *this, keeper_dr_);
	}
	else if (feature_type == "delta"sv)
	{
		return std::make_shared<impl::delta>(std::move(info), *this);
	}
	else if (feature_type == "shift_delta"sv)
	{
		return std::make_shared<impl::shift_delta>(std::move(info), *this);
	}
	else if (feature_type == "sma"sv)
	{
		return std::make_shared<impl::sma>(std::move(info), *this);
	}
	throw std::runtime_error("unknown feature type. feature id: "s + std::to_string(info.id_));
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
		return feature_map_.emplace(feature_id, _create_feature(feature_id)).first->second;
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature with id: {}. error: {}", feature_id, ex.what());
		throw;
	}
}
