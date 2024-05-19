#pragma once

#include <feature/framework.hpp>
#include <feature/abstract.hpp>
#include <feature/master_index.hpp>
#include <keeper/keeper.hpp>

namespace feature::impl
{
	//---------------------------------------------------------------------------------------------------------
	// stored
	// time series stored in database
	//---------------------------------------------------------------------------------------------------------
	class stored
		: public abstract
	{
	private:
		long long const data_reg_id_;
		std::shared_ptr<keeper::data_read> keeper_dr_;
	private:
		master_index const& _mi() const noexcept { return shop_.index(); }
		void _init();
	public:
		explicit stored(feature_info_t&& info, shop& shop, std::shared_ptr<keeper::data_read> keeper_dr);
		static feature_info_t rnd_from_template(feature_info_t const& feature_template, shop& shop);
	};
}