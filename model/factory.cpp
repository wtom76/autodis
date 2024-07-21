#include "pch.hpp"
#include "factory.hpp"
#include "file.hpp"
#include "model_011.hpp"

//---------------------------------------------------------------------------------------------------------
void autodis::model::factory::create_model_file(std::string const& model_type, std::filesystem::path const& out_path)
{
	if ("model_011"s == model_type)
	{
		autodis::model::model_011::create_model_file(model_type, 0, out_path);
	}
}
//---------------------------------------------------------------------------------------------------------
std::unique_ptr<autodis::model::abstract> autodis::model::factory::make_unique(std::filesystem::path const& in_path)
{
	file f{in_path};
	std::string const model_type{f.type_name()};

	if ("model_011"s == model_type)
	{
		return std::make_unique<autodis::model::model_011>(std::move(f));
	}
	return {};
}
