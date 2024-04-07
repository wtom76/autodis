#pragma once

#include <feature/framework.hpp>
#include <feature/abstract.hpp>
#include <feature/shop.hpp>

namespace feature
{
	class shop;
}

namespace feature::impl
{
	//---------------------------------------------------------------------------------------------------------
	// track
	// val(t) - val(t-depth)
	//---------------------------------------------------------------------------------------------------------
	class track
		: public abstract
	{
	private:
		shop& shop_;
		std::ptrdiff_t const depth_;
		std::shared_ptr<abstract> underlying_;
	private:
		value_t _evaluate(index_value_t idx_val) override;
	public:
		explicit track(nlohmann::json cfg, shop& shop);
	};
}