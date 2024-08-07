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
	// shift_delta
	// underlying_second->value(t + shift_second) - underlying_first->value(t + shift_first)
	//---------------------------------------------------------------------------------------------------------
	class shift_delta
		: public abstract
	{
	public:
		static constexpr type_id_t type_id_{3};
	private:
		std::pair<std::ptrdiff_t, std::ptrdiff_t> const shift_;
		std::pair<std::shared_ptr<abstract>, std::shared_ptr<abstract>> underlying_;
	private:
		master_index const& _mi() const noexcept { return shop_.index(); }
		void _init();
	public:
		explicit shift_delta(feature_info_t&& info, shop& shop);
		static feature_info_t rnd_from_template(feature_info_t const& feature_template, shop& shop);
	};
}