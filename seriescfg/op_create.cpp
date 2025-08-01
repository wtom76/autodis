#include "pch.hpp"
#include "op_create.hpp"

//---------------------------------------------------------------------------------------------------------
seriescfg::op_create::op_create(series_params params)
	: params_{std::move(params)}
{}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_create::_create_table(pqxx::work& t) const
{
	std::string const table_name{params_.data_table_name()};
	std::string const quoted_table_name{t.quote_name(table_name)};
	std::stringstream s;
	s
		<< "create table \"data\"."sv
		<< quoted_table_name
		<< " (idx int8 not null, f1 float8, f2 float8, f3 float8, f4 float8, f5 float8, "sv
		<< "constraint "sv << t.quote_name(table_name + "_un"s) << " unique (idx)); "sv
		<< "create unique index "sv << t.quote_name(table_name + "_idx_IDX"s) << " on \"data\"."sv << quoted_table_name << " using btree (idx);"sv
		<< "grant select on table \"data\"."sv << quoted_table_name << "to ml_viewer;"sv;

	t.exec(s.view());
}
//---------------------------------------------------------------------------------------------------------
std::array<std::int64_t, 5> seriescfg::op_create::_fill_data_registry(pqxx::work& t) const
{
	std::string const index_type{params_.index_type()};
	std::string const symbol{params_.symbol()};
	std::string const table_name{params_.data_table_name()};
	std::stringstream s;
	s
		<< "insert into metadata.data_registry (index_type_id, field_type_id, field_description, uri) values "sv
		<< "("sv << index_type << ", 100, "sv << t.quote(symbol + " open"s)		<< ", "sv << t.quote(table_name + "/f1"s) << "), "sv
		<< "("sv << index_type << ", 100, "sv << t.quote(symbol + " high"s)		<< ", "sv << t.quote(table_name + "/f2"s) << "), "sv
		<< "("sv << index_type << ", 100, "sv << t.quote(symbol + " low"s)		<< ", "sv << t.quote(table_name + "/f3"s) << "), "sv
		<< "("sv << index_type << ", 100, "sv << t.quote(symbol + " close"s)	<< ", "sv << t.quote(table_name + "/f4"s) << "), "sv
		<< "("sv << index_type << ", 101, "sv << t.quote(symbol + " volume"s)	<< ", "sv << t.quote(table_name + "/f5"s) << ") "sv
		<< "returning id"sv;

	pqxx::result const r{t.exec(s.view())};
	if (r.size() != 5)
	{
		throw std::runtime_error("failed to fill data registry. invalid entries number"s);
	}
	std::array<std::int64_t, 5> result;
	for (std::size_t i{0}; i != 5; ++i)
	{
		r[i][0].to(result[i]);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::vector<std::int64_t> seriescfg::op_create::_fill_source_registry(pqxx::work& t) const
{
	std::string const symbol{params_.symbol()};
	std::vector<source_params> src_params{params_.source_params_array()};
	std::stringstream s;
	s << "insert into metadata.source_registry (feed_id, uri, args) values "sv;
	for (size_t i{0}; i != src_params.size(); ++i)
	{
		source_params const& sp{src_params[i]};
		s << "("sv << sp.feed_id() << ", '"sv << sp.uri() << "', "sv << t.quote(sp.args()) << ")"sv
		  << ((i == src_params.size() - 1) ? ' ' : ',');
	}
	s << "returning id"sv;

	pqxx::result const r{t.exec(s.view())};
	if (r.size() != 2)
	{
		throw std::runtime_error("failed to fill source registry. invalid entries number"s);
	}
	std::vector<std::int64_t> src_ids(r.size(), 0);
	for (std::int64_t i{0}; i != r.size(); ++i)
	{
		r[i][0].to(src_ids[i]);
	}
	return src_ids;
}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_create::_fill_source_binding(pqxx::work& t, std::array<std::int64_t, 5> const& dr_ids, std::vector<std::int64_t> const& src_ids) const
{
	std::stringstream s;
	s << "insert into metadata.source_binding (data_id, source_id, feed_args) values "sv;
	for (size_t i{0}; i != src_ids.size(); ++i)
	{
		std::int64_t const src_id{src_ids[i]};
		s
			<< '(' << dr_ids[0] << ',' << src_id << ", 'open'), "sv
			<< '(' << dr_ids[1] << ',' << src_id << ", 'high'), "sv
			<< '(' << dr_ids[2] << ',' << src_id << ", 'low'), "sv
			<< '(' << dr_ids[3] << ',' << src_id << ", 'close'), "sv
			<< '(' << dr_ids[4] << ',' << src_id << ", 'volume')"sv
			<< ((i == src_ids.size() - 1) ? ' ' : ',');
	}

	t.exec(s.view());
}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_create::run(pqxx::connection& con)
{
	if (params_.type() != "moex_daily_json"sv && params_.type() != "moex_hour_json"sv)
	{
		SPDLOG_LOGGER_ERROR(log(), "unknown type '{}'", params_.type());
		throw std::runtime_error("failed to configure series"s);
	}
	
	pqxx::work t{con};
	_create_table(t);
	_fill_source_binding(t, _fill_data_registry(t), _fill_source_registry(t));
	t.commit();

	SPDLOG_LOGGER_INFO(log(), "created '{}' in '{}'", params_.symbol(), params_.data_table_name());
}
