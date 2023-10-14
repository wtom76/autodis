#include "pch.hpp"
#include "factory.hpp"
#include "feed/finam_daily_csv.hpp"
#include "feed/linear_generator.hpp"
#include "feed/moex_rest.hpp"
#include "source/file.hpp"
#include "source/dummy.hpp"
#include "source/rest.hpp"

//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::base> collector::factory::_create_file(std::string file_name)
{
	static const std::filesystem::path folder_path{shared::util::exe_dir_path() / "data_source"};
	return std::make_unique<source::file>(folder_path / file_name);
}
//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::feed> collector::factory::feed(keeper::metadata::feed_info const& info)
{
	feed_uri const uri{info.feed_uri_};
	if (uri.feed_name() == "finam_daily_csv")
	{
		return std::make_unique<feed::finam_daily_csv>(std::span<keeper::feed_args_t const>{info.feed_args_});
	}
	if (uri.feed_name() == "linear_generator")
	{
		return std::make_unique<feed::linear_generator>(info.feed_args_);
	}
	if (uri.feed_name() == "moex_rest")
	{
		return std::make_unique<feed::moex_rest>(info.feed_args_);
	}
	throw std::runtime_error("unknown feed in uri "s + uri.to_string());
}
//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::base> collector::factory::source(source_uri const& uri, std::string const& source_args)
{
	if (uri.type_name() == "file")
	{
		return _create_file(uri.name());
	}
	if (uri.type_name() == "dummy")
	{
		return std::make_unique<source::dummy>();
	}
	if (uri.type_name() == "rest")
	{
		return std::make_unique<source::rest>(source_args);
	}
	throw std::runtime_error("unknown source in uri "s + uri.to_string());
}
