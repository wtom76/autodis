#pragma once

#include <feature/framework.hpp>
#include <feature/abstract.hpp>
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
		void _init();
	public:
		explicit stored(feature_info_t&& info, std::shared_ptr<keeper::data_read> keeper_dr);
	};
}