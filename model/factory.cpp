#include "pch.hpp"
#include "factory.hpp"
#include "file.hpp"
#include "model_010.hpp"
#include "model_011.hpp"

//---------------------------------------------------------------------------------------------------------
static std::filesystem::path _model_file_path(std::string const& model_name)
{
	std::filesystem::path path{model_name};
	if (!path.has_extension())
	{
		path += ".json";
	}
	return path;
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::factory::create_model_file(std::string const& model_type, std::string const& model_name)
{
	std::filesystem::path const file_path{_model_file_path(model_name)};
	if (std::filesystem::exists(file_path))
	{
		SPDLOG_LOGGER_ERROR(shared::util::log(), "can't create '{}'. file already exists", file_path.native());
		return;
	}
	if ("model_010"s == model_type)
	{
		autodis::model::model_010::create_model_file(model_type, 0, file_path);
	}
	else if ("model_011"s == model_type)
	{
		autodis::model::model_011::create_model_file(model_type, 0, file_path);
	}
}
//---------------------------------------------------------------------------------------------------------
std::unique_ptr<autodis::model::abstract> autodis::model::factory::make_unique(std::string const& model_name)
{
	file f{_model_file_path(model_name)};
	std::string const model_type{f.type_name()};

	if ("model_010"s == model_type)
	{
		return std::make_unique<autodis::model::model_010>(std::move(f));
	} 
	else if ("model_011"s == model_type)
	{
		return std::make_unique<autodis::model::model_011>(std::move(f));
	}
	return {};
}
