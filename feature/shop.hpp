#pragma once

#include "framework.hpp"
#include "abstract.hpp"
#include "master_index.hpp"
#include <keeper/keeper.hpp>

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// shop
	//---------------------------------------------------------------------------------------------------------
	class shop
		: private shared::util::logged
	{
	// types
	private:
		using feature_map_t = std::unordered_map<std::int64_t, std::shared_ptr<abstract>>;
	// data
	private:
		std::shared_ptr<keeper::metadata> keeper_md_;
		std::shared_ptr<keeper::data_read> keeper_dr_;
		master_index master_index_;
		feature_map_t feature_map_;
	// methods
	private:
		std::shared_ptr<abstract> _create_feature(std::int64_t feature_id);
		std::shared_ptr<abstract> _create_feature(keeper::metadata::feature_info&& info);
	public:
		shop();
		master_index const& index() const noexcept { return master_index_; }
		// feature_id is id from feature DB table
		std::shared_ptr<abstract> feature(std::int64_t feature_id);
	};
}