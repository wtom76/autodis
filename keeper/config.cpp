#include "pch.h"
#include "config.h"

namespace keeper
{
	//---------------------------------------------------------------------------------------------------------
	void config::load()
	{
		std::ifstream f{"config.json"s};
		nlohmann::json j{nlohmann::json::parse(f)};
	}
	//---------------------------------------------------------------------------------------------------------
	void from_json(const nlohmann::json& j, config& dst)
	{
		j.at("db_connection"sv).get_to(dst.db_connection_);
	}
}
