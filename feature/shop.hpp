#pragma once

#include "abstract.hpp"
#include <keeper/keeper.hpp>

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// shop
	//---------------------------------------------------------------------------------------------------------
	class shop
	{
	// types
	private:
		using feature_map_t = std::unordered_map<std::string, std::shared_ptr<abstract>>;
	// data
	private:
		feature_map_t feature_map_;	
		std::shared_ptr<keeper::data_read> keeper_dr_;
	// methods
	private:
		std::shared_ptr<abstract> _create_feature(nlohmann::json cfg);
	public:
		shop();
		std::shared_ptr<abstract> feature(nlohmann::json cfg);
	};
}