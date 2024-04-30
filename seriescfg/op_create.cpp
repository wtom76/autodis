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
	std::string const symbol{params_.symbol()};
	std::string const table_name{params_.data_table_name()};
	std::stringstream s;
	s
		<< "insert into metadata.data_registry (index_type_id, field_type_id, field_description, uri) values "sv
		<< "(1, 100, "sv << t.quote(symbol + " open"s)	<< ", "sv << t.quote(table_name + "/f1"s) << "), "sv
		<< "(1, 100, "sv << t.quote(symbol + " high"s)	<< ", "sv << t.quote(table_name + "/f2"s) << "), "sv
		<< "(1, 100, "sv << t.quote(symbol + " low"s)	<< ", "sv << t.quote(table_name + "/f3"s) << "), "sv
		<< "(1, 100, "sv << t.quote(symbol + " close"s)	<< ", "sv << t.quote(table_name + "/f4"s) << "), "sv
		<< "(1, 101, "sv << t.quote(symbol + " vol"s)	<< ", "sv << t.quote(table_name + "/f5"s) << ") "sv
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
std::array<std::int64_t, 2> seriescfg::op_create::_fill_source_registry(pqxx::work& t) const
{
	std::string const symbol{params_.symbol()};
	std::stringstream s;
	s
		<< "insert into metadata.source_registry (feed_id, uri, args, pending, \"always\") values "sv
		<< "(1, "sv << t.quote("file/"s + symbol + "_daily"s) << ", NULL, false, false), "sv
		<< "(3, 'rest', "sv << t.quote(params_.rest_argument()) << ", true, true) "sv
		<< "returning id"sv;

	pqxx::result const r{t.exec(s.view())};
	if (r.size() != 2)
	{
		throw std::runtime_error("failed to fill source registry. invalid entries number"s);
	}
	std::array<std::int64_t, 2> result;
	for (std::size_t i{0}; i != 2; ++i)
	{
		r[i][0].to(result[i]);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_create::_fill_source_binding(pqxx::work& t, std::array<std::int64_t, 5> const& dr_ids, std::array<std::int64_t, 2> const& src_ids) const
{
	static const std::unordered_map<std::string /*board*/, std::array<std::string, 5>> feed_args{
		{"SNDX"s, {"/OPENVALUE"s, "/HIGH"s, "/LOW"s, "/LASTVALUE"s, "/VALTODAY"s}},
		{"default"s, {"/OPEN"s, "/HIGH"s, "/LOW"s, "/LAST"s, "/VOLTODAY"s}}
	};
	std::string const board{params_.board()};

	auto feed_args_i{feed_args.find(board)};
	if (feed_args_i == std::cend(feed_args))
	{
		feed_args_i = feed_args.find("default"s);
	}
	assert(feed_args_i != std::cend(feed_args));

	std::stringstream s;
	s
		<< "insert into metadata.source_binding (data_id, source_id, feed_args) values "sv
		<< '(' << dr_ids[0] << ',' << src_ids[0] << ", 'open'), "sv
		<< '(' << dr_ids[1] << ',' << src_ids[0] << ", 'high'), "sv
		<< '(' << dr_ids[2] << ',' << src_ids[0] << ", 'low'), "sv
		<< '(' << dr_ids[3] << ',' << src_ids[0] << ", 'close'), "sv
		<< '(' << dr_ids[4] << ',' << src_ids[0] << ", 'vol'), "sv
		<< '(' << dr_ids[0] << ',' << src_ids[1] << ',' << t.quote(board + feed_args_i->second[0]) << "), "sv
		<< '(' << dr_ids[1] << ',' << src_ids[1] << ',' << t.quote(board + feed_args_i->second[1]) << "), "sv
		<< '(' << dr_ids[2] << ',' << src_ids[1] << ',' << t.quote(board + feed_args_i->second[2]) << "), "sv
		<< '(' << dr_ids[3] << ',' << src_ids[1] << ',' << t.quote(board + feed_args_i->second[3]) << "), "sv
		<< '(' << dr_ids[4] << ',' << src_ids[1] << ',' << t.quote(board + feed_args_i->second[4]) << ") "sv;

	t.exec(s.view());
}
//---------------------------------------------------------------------------------------------------------
void seriescfg::op_create::run(pqxx::connection& con)
{
	if (params_.type() != "moex_ohlcv_daily"sv)
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
