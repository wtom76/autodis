#pragma once

#include "framework.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class file
	// serialization of model type, config and network data
	//---------------------------------------------------------------------------------------------------------
	class file
	{
	// data
	private:
		struct section_name;

	private:
		std::filesystem::path	file_path_;
		nlohmann::json			j_;

	// methods
	public:
		explicit file(std::filesystem::path file_path);	// open existing file
		explicit file(
			std::string type_name,
			std::int64_t model_id,
			std::filesystem::path file_path);			// create new file

		[[nodiscard]] std::string type_name() const;	// type name or exception
		[[nodiscard]] std::int64_t model_id() const;	// model id in DB or exception
		std::filesystem::path const& path() const noexcept { return file_path_; }
		nlohmann::json& config();						// config part of JSON
		nlohmann::json& features();						// for feature shop
		nlohmann::json& target();						// for feature shop
		nlohmann::json& network();						// network part of JSON file

		void store() const;								// stores JSON file or throws an exception
	};
}