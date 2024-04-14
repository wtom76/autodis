#include "pch.hpp"
#include "file.hpp"

//---------------------------------------------------------------------------------------------------------
// model_file
//---------------------------------------------------------------------------------------------------------
struct autodis::model::file::section_name
{
	static constexpr std::string_view type_name_{"type_name"};
	static constexpr std::string_view model_id_{"model_id"};
	static constexpr std::string_view config_{"config"};
	static constexpr std::string_view features_{"features"};
	static constexpr std::string_view target_{"target"};
	static constexpr std::string_view network_{"network"};
}; 
//---------------------------------------------------------------------------------------------------------
autodis::model::file::file(std::filesystem::path file_path)
	: file_path_{std::move(file_path)}
	, j_(nlohmann::json::value_t::object)	// can't use {} instead of () because {} will create array
{
	std::ifstream f{file_path_};
	if (f)
	{
		j_ = nlohmann::json::parse(f);
	}
}
//---------------------------------------------------------------------------------------------------------
autodis::model::file::file(std::string type_name, std::int64_t model_id, std::filesystem::path file_path)
	: file_path_{std::move(file_path)}
	, j_(nlohmann::json::value_t::object)	// can't use {} instead of () because {} will create array
{
	j_[section_name::type_name_] = type_name;
	j_[section_name::model_id_] = model_id;
	j_[section_name::features_] = {{}};
	j_[section_name::target_] = {};
}
//---------------------------------------------------------------------------------------------------------
std::string autodis::model::file::type_name() const
{
	return j_.at(section_name::type_name_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::int64_t autodis::model::file::model_id() const
{
	return j_.at(section_name::model_id_).get<std::int64_t>();
}
//---------------------------------------------------------------------------------------------------------
nlohmann::json& autodis::model::file::config()
{
	return j_[section_name::config_];
}
//---------------------------------------------------------------------------------------------------------
nlohmann::json& autodis::model::file::features()
{
	return j_[section_name::features_];
}
//---------------------------------------------------------------------------------------------------------
nlohmann::json& autodis::model::file::target()
{
	return j_[section_name::target_];
}
//---------------------------------------------------------------------------------------------------------
nlohmann::json& autodis::model::file::network()
{
	return j_[section_name::network_];
}
//---------------------------------------------------------------------------------------------------------
void autodis::model::file::store() const
{
	std::ofstream f{file_path_};
	if (!f)
	{
		throw std::runtime_error{"can't store model file "s + file_path_.native()};
	}
	f << j_;
}
