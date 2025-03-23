#include "pch.hpp"
#include "config.hpp"
#include <libgen.h>

namespace autodis
{
	//---------------------------------------------------------------------------------------------------------
	void config::load()
	{
		try
		{
			std::ifstream f{std::filesystem::current_path() / "autodis.json"s};
			nlohmann::json const j = nlohmann::json::parse(f);		// don't use {} here istead of =. https://github.com/nlohmann/json/issues/2204
			j.at("db_connection"sv).get_to(db_connection_);
			j.at("store_prediction_proc_name"sv).get_to(store_prediction_);
		}
		catch (std::exception const& ex)
		{
			throw std::runtime_error("failed to load autodis.json. "s + ex.what());
		}
	}
	//---------------------------------------------------------------------------------------------------------
	config& cfg()
	{
		static config cfg;
		return cfg;
	}
}
