#include "pch.hpp"
#include "metadata.hpp"
#include "config.hpp"

//---------------------------------------------------------------------------------------------------------
keeper::metadata::metadata(const config& cfg)
	: con_{cfg.db_connection_}
{
	// drop pending flag
	con_.prepare("dpf", "update metadata.data_registry set pending = false where id = $1");
}
//---------------------------------------------------------------------------------------------------------
std::vector<keeper::metadata::series_info> keeper::metadata::load()
{
	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select dr.id, dr.uri, fr.uri, sr.uri, dr.pending "
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
			rec[3].as<std::string>(),
			rec[4].as<bool>()
		);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void keeper::metadata::drop_pending_flag(series_info::id_t series_id)
{
	pqxx::work t{con_};
	t.exec_prepared("dpf", series_id);
	t.commit();
}
