#include "pch.hpp"
#include "config.hpp"
//#include <libgen.h>

namespace seriescfg
{
	//---------------------------------------------------------------------------------------------------------
	void config::load()
	{
		try
		{
			std::ifstream f{shared::util::exe_dir_path() / "seriescfg.json"s};
			nlohmann::json const j = nlohmann::json::parse(f);		// don't use {} here istead of =. https://github.com/nlohmann/json/issues/2204
			j.at("db_connection"sv).get_to(db_connection_);
		}
		catch (std::exception const& ex)
		{
			throw std::runtime_error("failed to load seriescfg.json. "s + ex.what());
		}
	}
}
