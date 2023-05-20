#include "pch.hpp"
#include "factory.hpp"
#include "feed/finam_daily_csv.hpp"
#include "feed/linear_generator.hpp"
#include "source/file.hpp"
#include "source/dummy.hpp"

//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::base> collector::factory::_create_file(std::string file_name)
{
	static const std::filesystem::path folder_path{"data_source"};
	return std::make_unique<source::file>(folder_path / file_name);
}
//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::feed> collector::factory::feed(keeper::metadata::feed_info const& info)
{
	feed_uri const uri{info.feed_uri_};
	if (uri.feed_name() == "finam_daily_csv")
	{
		return std::make_unique<feed::finam_daily_csv>(info.feed_args_);
	}
	if (uri.feed_name() == "linear_generator")
	{
		return std::make_unique<feed::linear_generator>(info.feed_args_);
	}
	throw std::runtime_error("unknown feed in uri "s + uri.to_string());
}
//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::base> collector::factory::source(source_uri const& uri)
{
	if (uri.type_name() == "file")
	{
		return _create_file(uri.name());
	}
	if (uri.type_name() == "dummy")
	{
		return std::make_unique<source::dummy>();
	}
	throw std::runtime_error("unknown source in uri "s + uri.to_string());
}
