#include "pch.hpp"
#include "op_delete.hpp"

//---------------------------------------------------------------------------------------------------------
seriescfg::op_delete::op_delete(series_params params)
	: params_{std::move(params)}
{}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_delete::_drop_table(pqxx::work& t) const
{
	std::string const table_name{params_.data_table_name()};
	std::stringstream s;
	s << "drop table \"data\"." << t.quote_name(table_name);
	t.exec(s.view());
}
//---------------------------------------------------------------------------------------------------------
std::array<std::int64_t, 5> seriescfg::op_delete::_delete_data_registry(pqxx::work& t) const
{
	std::string const symbol{params_.symbol()};
	std::string const table_name{params_.data_table_name()};
	std::stringstream s;
	s
		<< "delete from metadata.data_registry where uri in ("sv
		<< t.quote(table_name + "/f1"s) << ", "sv
		<< t.quote(table_name + "/f2"s) << ", "sv
		<< t.quote(table_name + "/f3"s) << ", "sv
		<< t.quote(table_name + "/f4"s) << ", "sv
		<< t.quote(table_name + "/f5"s) << ") "sv
		<< "returning id"sv;

	pqxx::result const r{t.exec(s.view())};
	if (r.size() != 5)
	{
		throw std::runtime_error("failed to delete data registry. invalid entries number"s);
	}
	std::array<std::int64_t, 5> result;
	for (std::size_t i{0}; i != 5; ++i)
	{
		r[i][0].to(result[i]);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::array<std::int64_t, 2> seriescfg::op_delete::_delete_source_binding(pqxx::work& t, std::array<std::int64_t, 5> const& dr_ids) const
{
	pqxx::result const r{t.exec_params(
		"with deleted as (delete from metadata.source_binding where data_id in ($1, $2, $3, $4, $5) returning source_id) select distinct source_id from deleted"s,
		dr_ids[0], dr_ids[1], dr_ids[2], dr_ids[3], dr_ids[4])};	// yes, it's laziness

	if (r.size() != 2)
	{
		throw std::runtime_error("failed to delete source binding. invalid entries number"s);
	}
	std::array<std::int64_t, 2> result;
	for (std::size_t i{0}; i != 2; ++i)
	{
		r[i][0].to(result[i]);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_delete::_delete_source_registry(pqxx::work& t, std::array<std::int64_t, 2> const& src_ids) const
{
	pqxx::result const r{t.exec_params("delete from metadata.source_registry where id in ($1, $2)"s, src_ids[0], src_ids[1])};
}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_delete::run(pqxx::connection& con)
{
	pqxx::work t{con};

	if (params_.type() != "moex_ohlcv_daily"sv)
	{
		SPDLOG_LOGGER_ERROR(log(), "unknown type '{}'", params_.type());
		return;
	}
	
	_delete_source_registry(t, _delete_source_binding(t, _delete_data_registry(t)));
	_drop_table(t);
	t.commit();

	SPDLOG_LOGGER_INFO(log(), "dropped '{}' in '{}'", params_.symbol(), params_.data_table_name());
}