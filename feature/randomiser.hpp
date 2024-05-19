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
	// data
	private:
		std::random_device random_device_;
		std::default_random_engine random_engine_{random_device_()};

	// methods
	public:
		//---------------------------------------------------------------------------------------------------------
		template <typename value_type>
		[[nodiscard]] value_type const& pick_random(std::vector<value_type> const& values)
		{
			assert(!values.empty());
			return values[std::uniform_int_distribution<std::size_t>{0, values.size() - 1}(random_engine_)];
		}
		//---------------------------------------------------------------------------------------------------------
		template <std::integral value_type>
		[[nodiscard]] value_type pick_random(value_type min, value_type max)
		{
			return std::uniform_int_distribution<value_type>{min, max}(random_engine_);
		}
	};
}