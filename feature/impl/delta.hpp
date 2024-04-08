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
	// delta
	// underlying_second->value(t + shift_second) - underlying_first->value(t + shift_first)
	// since peeking into future is forbidden, shift can be negatve only
	//---------------------------------------------------------------------------------------------------------
	class delta
		: public abstract
	{
	private:
		shop& shop_;
		std::pair<std::ptrdiff_t, std::ptrdiff_t> const shift_;
		std::pair<std::shared_ptr<abstract>, std::shared_ptr<abstract>> underlying_;
	private:
		value_t _evaluate(index_value_t idx_val) override;
	public:
		explicit delta(nlohmann::json cfg, shop& shop);
	};
}