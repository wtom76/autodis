#include "pch.hpp"
#include "config.hpp"
#include <libgen.h>

namespace keeper
{
	//---------------------------------------------------------------------------------------------------------
	void config::load()
	{
		try
		{
			std::ifstream f{std::filesystem::current_path() / "keeper.json"s};
			const nlohmann::json j = nlohmann::json::parse(f);		// don't use {} here istead of =. https://github.com/nlohmann/json/issues/2204
			j.at("db_connection"sv).get_to(db_connection_);
			j.at("feature_table_name"sv).get_to(feature_table_name_);
		}
		catch (std::exception const& ex)
		{
			throw std::runtime_error("failed to load keeper.json. "s + ex.what());
		}
	}
}
