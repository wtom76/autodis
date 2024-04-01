#include "pch.hpp"
#include "shop.hpp"
#include "impl/stored.hpp"

//---------------------------------------------------------------------------------------------------------
feature::shop::shop()
	: keeper_dr_{[]
		{
			keeper::config keeper_cfg;
			keeper_cfg.load();
			return std::make_shared<keeper::data_read>(keeper_cfg);
		}()}
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
	throw std::runtime_error("failed to create feature from "s + cfg.dump());
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
	return feature_map_.emplace(cfg_str, _create_feature(cfg)).first->second;
}
