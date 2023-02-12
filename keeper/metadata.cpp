#include "pch.h"
#include "metadata.h"
#include "config.h"

//---------------------------------------------------------------------------------------------------------
keeper::metadata::metadata(const config& cfg)
	: con_{cfg.db_connection_}
{
}
//---------------------------------------------------------------------------------------------------------
std::vector<keeper::metadata::series_info> keeper::metadata::load()
{
	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select dr.id, dr.uri, fr.uri, sr.uri "
		"from metadata.data_registry dr "
		"inner join metadata.feed_registry fr on dr.feed_id = fr.id "
		"inner join metadata.source_registry sr on dr.source_id = sr.id")
	};
	std::vector<keeper::metadata::series_info> result;
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
