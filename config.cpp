#include "pch.hpp"
#include "config.hpp"
#include <libgen.h>

namespace autodis
{
	//---------------------------------------------------------------------------------------------------------
	void config::load()
	{
		std::ifstream f{shared::util::exe_dir_path() / "autodis.json"s};
		const nlohmann::json j = nlohmann::json::parse(f);		// don't use {} here istead of =. https://github.com/nlohmann/json/issues/2204
		j.at("db_connection"sv).get_to(db_connection_);
	}
	//---------------------------------------------------------------------------------------------------------
	config& cfg()
	{
		static config cfg;
		return cfg;
	}
}
