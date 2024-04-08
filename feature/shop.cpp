#include "pch.hpp"
#include "shop.hpp"
#include "impl/stored.hpp"
#include "impl/delta.hpp"
#include "impl/sma.hpp"

//---------------------------------------------------------------------------------------------------------
feature::shop::shop()
	: keeper_dr_{[]
		{
			keeper::config keeper_cfg;
			keeper_cfg.load();
			return std::make_shared<keeper::data_read>(keeper_cfg);
		}()}
	, master_index_{*keeper_dr_}
{}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::_create_feature(nlohmann::json cfg)
{
	std::string_view feature_type;
	cfg.at("type"sv).get_to(feature_type);

	if (feature_type == "stored"sv)
	{
		return std::make_shared<impl::stored>(std::move(cfg), keeper_dr_);
	}
	else if (feature_type == "delta"sv)
	{
		return std::make_shared<impl::delta>(std::move(cfg), *this);
	}
	else if (feature_type == "sma"sv)
	{
		return std::make_shared<impl::sma>(std::move(cfg), *this);
	}
	throw std::runtime_error("unknown feature type: "s + cfg.dump());
}
//---------------------------------------------------------------------------------------------------------
std::shared_ptr<feature::abstract> feature::shop::feature(nlohmann::json cfg)
{
	std::string const cfg_str{cfg.dump()};
	auto const feature_map_i{feature_map_.find(cfg_str)};
	if (feature_map_i != feature_map_.cend())
	{
		return feature_map_i->second;
	}
	try
	{
		return feature_map_.emplace(cfg_str, _create_feature(cfg)).first->second;
	}
	catch (std::exception const& ex)
	{
		SPDLOG_LOGGER_ERROR(log(), "failed to create feature from {}. error: {}", cfg.dump(), ex.what());
		throw;
	}
}
