#pragma once

#include <feature/framework.hpp>
#include <feature/abstract.hpp>
#include <feature/shop.hpp>

namespace feature::impl
{
	//---------------------------------------------------------------------------------------------------------
	// stored
	// time series stored in database
	//---------------------------------------------------------------------------------------------------------
	class track
		: public abstract
	{
	private:
		std::size_t const depth_;
		std::shared_ptr<abstract> underlying_;
	private:
		value_t _evaluate(index_value_t idx_val) override;
	public:
		explicit track(nlohmann::json cfg, shop& shop);
	};
}