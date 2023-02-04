#include "pch.h"
#include "metadata.h"
#include "config.h"

//---------------------------------------------------------------------------------------------------------
keeper::metadata::metadata(const config& cfg)
	: con_{cfg.db_connection_}
{
}

//---------------------------------------------------------------------------------------------------------
keeper::metadata::data_info keeper::metadata::load_series_info(long long id)
{
	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select dr.uri, fr.uri, sr.uri "
		"from metadata.data_registry dr "
		"inner join metadata.feed_registry fr on dr.feed_id = fr.id "
		"inner join metadata.source_registry sr on dr.source_id = sr.id "
		"where dr.id = $1",
		id)
	};

	if (r.empty())
	{
		return {};
	}
	const pqxx::row& rec{r[0]};
	return
		{
			id,
			rec[0].as<std::string>(),
			rec[1].as<std::string>(),
			rec[2].as<std::string>()
		};
}

//---------------------------------------------------------------------------------------------------------
std::vector<keeper::metadata::data_info> keeper::metadata::load_all_series_info()
{
	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select dr.id, dr.uri, fr.uri, sr.uri "
		"from metadata.data_registry dr "
		"inner join metadata.feed_registry fr on dr.feed_id = fr.id "
		"inner join metadata.source_registry sr on dr.source_id = sr.id")
	};
	std::vector<keeper::metadata::data_info> result;
	if (r.empty())
	{
		return result;
	}
	result.reserve(r.size());
	for (const pqxx::row& rec : r)
	{
		result.emplace_back(
			rec[0].as<long long>(),
			rec[1].as<std::string>(),
			rec[2].as<std::string>(),
			rec[3].as<std::string>()
		);
	}
	return result;
}
