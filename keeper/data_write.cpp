#include "pch.hpp"
#include "data_write.hpp"
#include "config.hpp"

//---------------------------------------------------------------------------------------------------------
keeper::data_write::data_write(config const& cfg, std::vector<data_uri> dest_uri)
	: con_{cfg.db_connection_}
	, dest_uri_{std::move(dest_uri)}
{
	_prepare_queries();
}
//---------------------------------------------------------------------------------------------------------
// insert into "data"."<table1>" (idx, "<field>", ...) values ($1, ...)
// on conflict (idx) do update set "<field>" = excluded."<field>", ...;
// insert into "data"."<table2>" (idx, "<field>", ...) values ($1, ...)
// on conflict (idx) do update set "<field>" = excluded."<field>", ...;
void keeper::data_write::_prepare_queries()
{
	struct table_info
	{
		std::string field_list_;			// ", <field>", ...
		std::string value_idx_;				// $1, $2, ...
		std::string field_update_list_;		// "<field>" = excluded."<field>", ...
	};
	std::unordered_map<std::string /*table name*/, table_info> table_map;

	int field_idx{2};	// starts from 2, 1 is for index
	for (data_uri const& du : dest_uri_)
	{
		table_info& info{table_map[du.table_name()]};

		if (!info.field_list_.empty())
		{
			info.field_list_ += ", "s;
			info.value_idx_ += ", "s;
			info.field_update_list_ += ", "s;
		}
		info.field_list_ += "\""s;
		info.field_list_ += du.field_name();
		info.field_list_ += "\""s;

		info.value_idx_ += "$"s;
		info.value_idx_ += std::to_string(field_idx++);

		info.field_update_list_ += "\""s;
		info.field_update_list_ += du.field_name();
		info.field_update_list_ += "\" = excluded.\""s;
		info.field_update_list_ += du.field_name();
		info.field_update_list_ += '\"';
	}

	std::string query;
	for (auto const& pr : table_map)
	{
		query += "insert into \"data\".\""s;
		query += pr.first;
		query += "\" (\"idx\", "s;
		query += pr.second.field_list_;
		query += ") values ($1, "s;
		query += pr.second.value_idx_;
		query += ") on conflict (\"idx\") do update set "s;
		query += pr.second.field_update_list_;
		query += ';';
	}

	con_.prepare("store"s, query);
}
//---------------------------------------------------------------------------------------------------------
void keeper::data_write::_commit_buf()
{
	pqxx::work t{con_};
	for (auto pr : data_)
	{
		pqxx::params p;
		p.append(pr.first);
		for (auto val : pr.second)
		{
			p.append(val);
		}
		t.exec("store"sv, p);
	}
	t.commit();
	data_.clear();
}
//---------------------------------------------------------------------------------------------------------
void keeper::data_write::add(std::pair<long long, std::vector<double>>&& idx_val)
{
	constexpr std::size_t batch_size{1024};

	assert(idx_val.second.size() == dest_uri_.size());

	data_.emplace_back(std::move(idx_val));

	if (data_.size() >= batch_size)
	{
		_commit_buf();
	}
}
//---------------------------------------------------------------------------------------------------------
void keeper::data_write::finish()
{
	if (data_.size() > 0)
	{
		_commit_buf();
	}
}
