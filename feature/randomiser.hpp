#pragma once

#include "framework.hpp"

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// randomiser
	//---------------------------------------------------------------------------------------------------------
	class randomiser
		: private shared::util::logged
	{
	// types
	private:
		using feature_meta_t = std::vector<feature_info_t>;
		using type_map_t = std::unordered_map<std::int32_t, feature_meta_t>;
	
	// data
	private:
		type_map_t type_map_;

		std::random_device random_device_;
		std::default_random_engine random_engine_{random_device_()};

	// methods
	private:
		//---------------------------------------------------------------------------------------------------------
		template <typename value_type>
		[[nodiscard]] value_type const& _pick_random(std::vector<value_type> const& values)
		{
			assert(!values.empty());
			return values[std::uniform_int_distribution<std::size_t>{0, values.size() - 1}(random_engine_)];
		}

		void _verify_typeset(std::vector<std::int32_t> const& type_ids, std::string const& label);
		feature_info_t _spec_delta(feature_info_t const& feature_template);
	public:
		randomiser();
		feature_info_t specialise(feature_info_t const& feature_template);
	};
}