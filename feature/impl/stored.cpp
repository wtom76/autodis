#include "../pch.hpp"
#include "stored.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
void feature::impl::stored::_init()
{
	shared::data::frame df;
	keeper_dr_->read(std::vector{data_reg_id_}, df);
	auto index_i{std::cbegin(df.index())};
	auto const index_e{std::cend(df.index())};
	auto series_i{std::cbegin(df.series(0))};
	while (index_i != index_e)
	{
		data_.emplace(*index_i++, *series_i++);
	}
}
//---------------------------------------------------------------------------------------------------------
feature::impl::stored::stored(feature_info_t&& info, std::shared_ptr<keeper::data_read> keeper_dr)
	: abstract{std::move(info)}
	, data_reg_id_{cfg().at("data_reg_id").get<long long>()}
	, keeper_dr_{std::move(keeper_dr)}
{
	_set_bounds(keeper_dr_->read_bounds(cfg().at("data_reg_id").get<long long>()));
	_init();
}
