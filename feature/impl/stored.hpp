#pragma once
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
		value_t _evaluate(index_value_t idx_val) override;
	public:
		explicit stored(nlohmann::json cfg, std::shared_ptr<keeper::data_read> keeper_dr);
	};
}