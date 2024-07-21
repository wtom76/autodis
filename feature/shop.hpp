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
	// TODO: keep calculated features in DB
	//---------------------------------------------------------------------------------------------------------
	class shop
		: private shared::util::logged
	{
	// types
	private:
		using feature_map_t = std::unordered_map<std::int64_t, std::shared_ptr<abstract>>;

		using feature_meta_t = std::vector<feature_info_t>;

	// data
	private:
		std::shared_ptr<keeper::metadata>	keeper_md_;
		std::shared_ptr<keeper::data_read>	keeper_dr_;
		std::int64_t						feature_set_id_{0};		// restricts random feature selection
		master_index						master_index_;
		feature_map_t						feature_map_;
		randomiser							randomiser_;

	// methods
	private:
		feature_info_t _load_feature_info(std::int64_t feature_id) const;
		feature_info_t _rnd_from_template(feature_info_t const& feature_template);
		std::shared_ptr<abstract> _create_feature(feature_info_t&& info);
		std::shared_ptr<abstract> _existing(feature_info_t const& info) const;
		std::shared_ptr<abstract> _feature(nlohmann::json const& fj);

	public:
		shop();
		shop(std::int64_t feature_set_id);
		master_index const& index() const noexcept { return master_index_; }
		// returns feature described by json
		// fj may be feature_id from DB or json representation of feature_info_t
		// in case of json representation of feature_info_t it should not be template
		// TODO: existing in shop features without DB id but created from feature_info_t json (template or not) should be identified and reused
		std::shared_ptr<abstract> feature(nlohmann::json const& fj);

		[[nodiscard]] feature_info_t random_info(std::vector<std::int64_t> const& feature_ids);
		[[nodiscard]] feature_info_t random_info_of_type(std::int32_t feature_type_id);

		template <std::integral value_type>
		[[nodiscard]] value_type random_value(std::vector<value_type> const& values){ return randomiser_.pick_random(values); }
		template <std::integral value_type>
		[[nodiscard]] value_type random_value(value_type min, value_type max){ return randomiser_.pick_random(min, max); }
	};
}