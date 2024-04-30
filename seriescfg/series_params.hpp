#pragma once

#include "framework.hpp"

namespace seriescfg
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class series_params
	{
	private:
		struct section_name;
	private:
		nlohmann::json j_;
	public:
		explicit series_params(std::filesystem::path file_path);	// open existing
		explicit series_params();									// create empty
		void store(std::filesystem::path file_path) const;			// stores JSON file or throws an exception

		std::string type() const;
		std::string data_table_name() const;
		std::string board() const;
		std::string symbol() const;
		std::string rest_argument() const;
	};
}