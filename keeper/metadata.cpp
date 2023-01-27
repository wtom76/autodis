#include "pch.h"
#include "metadata.h"
#include "config.h"

//---------------------------------------------------------------------------------------------------------
keeper::metadata::metadata(const config& cfg)
	: con_{cfg.db_connection_}
{
}

//---------------------------------------------------------------------------------------------------------
keeper::metadata::data_info keeper::metadata::load_data_info(long long id)
{
	pqxx::work t{con_};
	const pqxx::result r{t.exec_params(
		"select dr.table_name, dr.field_name, fr.uri feed_uri "
		"from metadata.data_registry dr "
		"inner join metadata.feed_registry fr on dr.feed_id = fr.id "
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
