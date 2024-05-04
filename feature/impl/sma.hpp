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
	// sma
	// simple moving average
	//---------------------------------------------------------------------------------------------------------
	class sma
		: public abstract
	{
	// data
	private:
		shop& shop_;
		std::ptrdiff_t const period_;
		std::shared_ptr<abstract> underlying_;
	// methods
	private:
		value_t _under_val_out(index_value_t idx_val) const;
		value_t _calc_from_scratch(index_value_t idx_val) const;
		value_t _calc(index_value_t idx_val) const;
		value_t _evaluate(index_value_t idx_val) override;
	public:
		explicit sma(feature_info_t&& info, shop& shop);
	};
}