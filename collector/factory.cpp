#include "pch.h"
#include "factory.h"
#include "feed/finam_daily_csv.h"
#include "source/file.h"

//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::base> collector::factory::_create_file(std::string file_name)
{
	static const std::filesystem::path folder_path{"data_source"};
	return std::make_unique<source::file>(folder_path / file_name);
}
//----------------------------------------------------------------------------------------------------------
std::unique_ptr<collector::source::feed> collector::factory::feed(feed_uri const& uri)
{
	if (uri.feed_name() == "finam_daily_csv")
	{
		return std::make_unique<feed::finam_daily_csv>(uri.field_name());
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
	throw std::runtime_error("unknown source in uri "s + uri.to_string());
}
