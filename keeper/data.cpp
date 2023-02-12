#include "pch.h"
#include "data.h"
#include "config.h"

//---------------------------------------------------------------------------------------------------------
keeper::data::data(config const& cfg, data_uri const& dest_uri)
	: con_{cfg.db_connection_}
{
	con_.prepare("store"s,
		"insert into \"data\".\""s + dest_uri.table_name() + "\" (idx, \""s + dest_uri.field_name() +
		"\") values ($1, $2) on conflict (idx) do update set \""s + dest_uri.field_name() + "\" = excluded.\""s + dest_uri.field_name() + '\"');
}
//---------------------------------------------------------------------------------------------------------
void keeper::data::add(std::pair<long long, double> idx_val)
{
	constexpr std::size_t batch_size{1024};

	data_.emplace_back(idx_val);

	if (data_.size() >= batch_size)
	{
		_commit_buf();
	}
}
//---------------------------------------------------------------------------------------------------------
void keeper::data::finish()
{
	if (data_.size() > 0)
	{
		_commit_buf();
	}
}
//---------------------------------------------------------------------------------------------------------
void keeper::data::_commit_buf()
{
	pqxx::work t{con_};
	for (auto pr : data_)
	{
		pqxx::params p;
		p.append(pr.first);
		p.append(pr.second);
		t.exec_prepared("store"s, p);
	}
	t.commit();
	data_.clear();
}