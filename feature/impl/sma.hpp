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
		std::ptrdiff_t const period_;
		std::shared_ptr<abstract> underlying_;
	// methods
	private:
		master_index const& _mi() const noexcept { return shop_.index(); }
		void _init();
	public:
		explicit sma(feature_info_t&& info, shop& shop);
		static feature_info_t rnd_from_template(feature_info_t const& feature_template, shop& shop);
	};
}