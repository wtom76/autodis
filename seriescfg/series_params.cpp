#include "pch.hpp"
#include "series_params.hpp"

//---------------------------------------------------------------------------------------------------------
struct seriescfg::source_params::param_name
{
	static constexpr std::string_view feed_id_{"feed_id"};
	static constexpr std::string_view uri_{"uri"};
	static constexpr std::string_view args_{"args"};
}; 
//---------------------------------------------------------------------------------------------------------
seriescfg::source_params::source_params(nlohmann::json j)
	: j_(std::move(j))	// can't use {} instead of () because {} will create array
{}
//---------------------------------------------------------------------------------------------------------
seriescfg::source_params::source_params()
	: j_({
			{param_name::feed_id_, "5"s},
			{param_name::uri_, "rest_by_chunks"s},
			{param_name::args_, "https://iss.moex.com/iss/engines/stock/markets/index/boards/SNDX/securities/IMOEX/candles.json?interval=24&start=0"s}
		})
{}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::source_params::feed_id() const
{
	return j_.at(param_name::feed_id_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::source_params::uri() const
{
	return j_.at(param_name::uri_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::source_params::args() const
{
	return j_.at(param_name::args_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
struct seriescfg::series_params::param_name
{
	static constexpr std::string_view data_table_{"data_table_name"};
	static constexpr std::string_view index_type_{"index_type_id"};
	static constexpr std::string_view symbol_{"symbol"};
	static constexpr std::string_view type_{"type_name"};
	static constexpr std::string_view source_params_{"source_params"};
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
			{param_name::data_table_, "000000"s},
			{param_name::index_type_, "4"s},
			{param_name::symbol_, "IMOEX"s},
			{param_name::type_, "moex_hour_json"s},
			{param_name::source_params_, std::vector<nlohmann::json>{source_params{}.j_}}
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
std::string seriescfg::series_params::data_table_name() const
{
	return j_.at(param_name::data_table_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::index_type() const
{
	return j_.at(param_name::index_type_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::symbol() const
{
	return j_.at(param_name::symbol_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::string seriescfg::series_params::type() const
{
	return j_.at(param_name::type_).get<std::string>();
}
//---------------------------------------------------------------------------------------------------------
std::vector<seriescfg::source_params> seriescfg::series_params::source_params_array() const
{
	auto j = j_.at(param_name::source_params_);
	std::vector<source_params> res;
	res.reserve(j.size());
	for (auto const& sp_j : j)
	{
		res.push_back(source_params{sp_j});
	}
	return res;
}
