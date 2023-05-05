#include "pch.hpp"
#include "data_read.hpp"
#include "config.hpp"

//---------------------------------------------------------------------------------------------------------
keeper::data_read::data_read(config const& cfg)
	: con_{cfg.db_connection_}
{
}
//---------------------------------------------------------------------------------------------------------
keeper::data_read::table_fields_map_t keeper::data_read::_field_map(std::vector<data_uri> const& src_uri) const
{
	table_fields_map_t result;
	for (auto const& uri : src_uri)
	{
		result[uri.table_name()].emplace_back(uri.field_name());
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::string keeper::data_read::_field_list(std::vector<std::string> const& fields) const
{
	std::string result;
	for (std::string const& field : fields)
	{
		if (!result.empty())
		{
			result += ", "sv;
		}
		result += '\"';
		result += field;
		result += '\"';
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void keeper::data_read::read(std::vector<data_uri> const& src_uri, shared::data::frame& dest)
{
	table_fields_map_t request{_field_map(src_uri)};
	if (request.empty() || request.size() > 1)
	{
		assert(request.empty()); // TODO: otherwise need to join different tables
		return;
	}

	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select \"idx\","s + _field_list(request.begin()->second) + " from \"data\".\""s + request.begin()->first + "\" order by idx asc"s)
	};

	{
		dest.clear();
		dest.resize(r.size());
		for (std::string const& field : request.begin()->second)
		{
			dest.create_series(field);
		}
	}
	
	static_assert(std::numeric_limits<double>::has_signaling_NaN);

	std::size_t row_idx{0};
	for (pqxx::row const& rec : r)
	{
		rec[0].to(dest.index()[row_idx]);
		for (std::size_t field_idx{0}; field_idx != dest.col_count(); ++field_idx)
		{
			rec[field_idx + 1].to(dest.series(field_idx)[row_idx], shared::data::nan);
		}
		++row_idx;
	}
}
