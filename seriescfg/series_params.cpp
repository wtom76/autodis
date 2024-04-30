#include "pch.hpp"
#include "series_params.hpp"

//---------------------------------------------------------------------------------------------------------
struct seriescfg::series_params::section_name
{
	static constexpr std::string_view type_{"type_name"};
	static constexpr std::string_view data_table_{"data_table_name"};
	static constexpr std::string_view symbol_{"symbol"};
	static constexpr std::string_view board_{"board"};
	static constexpr std::string_view rest_arg_{"rest_argument"};
}; 
//---------------------------------------------------------------------------------------------------------
seriescfg::series_params::series_params(std::filesystem::path file_path)
	: j_(nlohmann::json::value_t::object)	// can't use {} instead of () because {} will create array
{
	std::ifstream f{file_path};
	if (f)
	{
		j_ = nlohmann::json::parse(f);
	}
}
//---------------------------------------------------------------------------------------------------------
seriescfg::series_params::series_params()
	: j_({
			{section_name::type_, "moex_ohlcv_daily"s},
			{section_name::data_table_, "000000"s},
			{section_name::board_, "instrument board"s},
			{section_name::symbol_, "instrument symbol"s},
			{section_name::rest_arg_, "https://iss.moex.com/iss/engines/..."s}
		})
{}
//---------------------------------------------------------------------------------------------------------
void seriescfg::series_params::store(std::filesystem::path file_path) const
{
	std::ofstream f{file_path};
	if (!f)
	{
		throw std::runtime_error{"can't store series_params to "s + file_path.native()};
	}
	f << j_;
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::type() const
{
	return j_.at(section_name::type_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::data_table_name() const
{
	return j_.at(section_name::data_table_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::board() const
{
	return j_.at(section_name::board_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::symbol() const
{
	return j_.at(section_name::symbol_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::rest_argument() const
{
	return j_.at(section_name::rest_arg_).get<std::string>();
}
