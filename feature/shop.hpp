#pragma once

#include "abstract.hpp"
#include "master_index.hpp"
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
		std::shared_ptr<keeper::data_read> keeper_dr_;
		master_index master_index_;
		feature_map_t feature_map_;
	// methods
	private:
		std::shared_ptr<abstract> _create_feature(nlohmann::json cfg);
	public:
		shop();
		master_index const& index() const noexcept { return master_index_; }
		std::shared_ptr<abstract> feature(nlohmann::json cfg);
	};
}