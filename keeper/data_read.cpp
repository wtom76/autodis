#include "pch.hpp"
#include "data_read.hpp"
#include "metadata.hpp"

namespace
{
	using table_fields_map_t = std::unordered_map<std::string /*table name*/, std::vector<std::string> /*field list*/>;

	//---------------------------------------------------------------------------------------------------------
	table_fields_map_t field_map(std::vector<keeper::data_uri> const& uri_list)
	{
		table_fields_map_t result;
		for (auto const& uri : uri_list)
		{
			result[uri.table_name()].emplace_back(uri.field_name());
		}
		return result;
	}
	//---------------------------------------------------------------------------------------------------------
	std::string field_list(std::vector<std::string> const& fields)
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
}

//---------------------------------------------------------------------------------------------------------
keeper::data_read::data_read(config const& cfg)
	: cfg_{cfg}
	, con_{cfg.db_connection_}
{
}
//---------------------------------------------------------------------------------------------------------
void keeper::data_read::read(std::vector<data_uri> const& src_uri, shared::data::frame& dest)
{
	table_fields_map_t request{field_map(src_uri)};
	if (request.empty() || request.size() > 1)
	{
		assert(request.empty()); // TODO: otherwise need to join different tables
		return;
	}

	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select \"idx\","s + field_list(request.begin()->second) + " from \"data\".\""s + request.begin()->first + "\" order by idx asc"s)
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
//---------------------------------------------------------------------------------------------------------
void keeper::data_read::read(std::vector<long long> const& data_reg_ids, shared::data::frame& dest)
{
	std::vector<metadata::data_info> metainfo;
	{
		metadata md{cfg_};
		md.load_data_info(data_reg_ids, metainfo);
	}

	for (metadata::data_info const& mf : metainfo)
	{
		pqxx::work t{con_};
		const pqxx::result r{t.exec_params(
			"select \"idx\","s + mf.data_uri_.field_name() + " from \"data\".\""s + mf.data_uri_.table_name() + "\" order by idx asc"s)
		};

		shared::data::frame one_table_df;
		one_table_df.clear();
		one_table_df.resize(r.size());
		one_table_df.create_series(mf.description_);
		
		static_assert(std::numeric_limits<double>::has_signaling_NaN);

		std::size_t row_idx{0};
		for (pqxx::row const& rec : r)
		{
			rec[0].to(one_table_df.index()[row_idx]);
			rec[1].to(one_table_df.series(0)[row_idx], shared::data::nan);
			++row_idx;
		}

		dest.outer_join(std::move(one_table_df));
	}
}