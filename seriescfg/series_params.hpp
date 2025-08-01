#pragma once

#include "framework.hpp"

namespace seriescfg
{
	//---------------------------------------------------------------------------------------------------------
	// source_params
	//---------------------------------------------------------------------------------------------------------
	class source_params
	{
	private:
		struct param_name;
	public:
		nlohmann::json j_;
	public:
		explicit source_params(nlohmann::json j);			// from existing
		explicit source_params();							// create empty

		std::string feed_id() const;
		std::string uri() const;
		std::string args() const;
	};
	//---------------------------------------------------------------------------------------------------------
	// series_params
	//---------------------------------------------------------------------------------------------------------
	class series_params
	{
	private:
		struct param_name;
	private:
		nlohmann::json j_;
	public:
		explicit series_params(std::filesystem::path file_path);	// open existing
		explicit series_params();									// create empty
		void store(std::filesystem::path file_path) const;			// stores JSON file or throws an exception

		std::string data_table_name() const;
		std::string index_type() const;
		std::string symbol() const;
		std::string type() const;
		std::vector<source_params> source_params_array() const;
	};
}