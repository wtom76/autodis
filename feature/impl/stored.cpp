#include "../pch.hpp"
#include "stored.hpp"

//---------------------------------------------------------------------------------------------------------
feature::abstract::value_t feature::impl::stored::_evaluate(index_value_t idx_val)
{
	if (!bounds_.test(idx_val))
	{
		throw std::runtime_error("an index out of bounds"s);
	}
	shared::data::frame df;
	keeper_dr_->read(std::vector{data_reg_id_}, df);
	auto index_i{std::cbegin(df.index())};
	auto const index_e{std::cend(df.index())};
	auto series_i{std::cbegin(df.series(0))};
	while (index_i != index_e)
	{
		data_.emplace(*index_i++, *series_i++);
	}
	auto const data_i{data_.find(idx_val)};
	return data_i != data_.cend() ? data_i->second : shared::data::nan;
}
//---------------------------------------------------------------------------------------------------------
feature::impl::stored::stored(nlohmann::json cfg, std::shared_ptr<keeper::data_read> keeper_dr)
	: abstract{std::move(cfg)}
	, data_reg_id_{cfg_.at("data_reg_id").get<long long>()}
	, keeper_dr_{std::move(keeper_dr)}
{
	_set_bounds(keeper_dr->read_bounds(cfg.at("data_reg_id").get<long long>()));
}
