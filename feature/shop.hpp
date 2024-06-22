#pragma once

#include "framework.hpp"
#include "abstract.hpp"
#include "master_index.hpp"
#include "randomiser.hpp"
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

		using feature_meta_t = std::vector<feature_info_t>;
		using type_map_t = std::unordered_map<std::int32_t, feature_meta_t>;

	// data
	private:
		std::shared_ptr<keeper::metadata>	keeper_md_;
		std::shared_ptr<keeper::data_read>	keeper_dr_;
		master_index						master_index_;
		feature_map_t						feature_map_;

		type_map_t							type_map_;
		randomiser							randomiser_;

	// methods
	private:
		feature_info_t _load_feature_info(std::int64_t feature_id) const;
		std::string_view _feature_type(feature_info_t const& feature_template);
		feature_info_t _rnd_from_template(feature_info_t const& feature_template);
		std::shared_ptr<abstract> _create_feature(feature_info_t&& info);
		std::shared_ptr<abstract> _existing(std::int64_t feature_id);
		std::shared_ptr<abstract> _feature(nlohmann::json& fj);

	public:
		shop();
		master_index const& index() const noexcept { return master_index_; }
		// feature_id is id from feature DB table
		std::shared_ptr<abstract> feature(std::int64_t feature_id);
		// feature from json wich may be feature_id, feature formula or feature formula template
		std::shared_ptr<abstract> feature(nlohmann::json& fj);

		void verify_typeset(std::vector<std::int32_t> const& type_ids, std::string const& label);
		template <std::integral value_type>
		[[nodiscard]] value_type pick_random(std::vector<value_type> const& type_ids){ return randomiser_.pick_random(type_ids); }
		template <std::integral value_type>
		[[nodiscard]] value_type pick_random(value_type min, value_type max){ return randomiser_.pick_random(min, max); }
		[[nodiscard]] feature_info_t const& random_feature_info(std::int32_t feature_type_id);
	};
}