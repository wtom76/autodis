#include "pch.hpp"
#include "moex_rest.hpp"
#include <pugixml.hpp>

//---------------------------------------------------------------------------------------------------------
collector::feed::moex_rest::moex_rest(std::vector<std::string> feed_args)
	: feed_args_{std::move(feed_args)}
{}
//---------------------------------------------------------------------------------------------------------
// "2023-05-21..." -> "2023052121..." -> 20230521
void collector::feed::moex_rest::_parse_date(std::string dt_str)
{
	if (dt_str.size() < 10)
	{
		throw std::runtime_error{"unknown SYSTIME format: \""s + dt_str + "\""};
	}
	dt_str[4] = dt_str[5];
	dt_str[5] = dt_str[6];
	dt_str[6] = dt_str[8];
	dt_str[7] = dt_str[9];
	std::from_chars_result res{std::from_chars(dt_str.data(), dt_str.data() + 8, result_.date_)};
	if (res.ec != std::errc{})
	{
		throw std::runtime_error{"failed to parse SYSTIME: \""s + dt_str + "\""};
	}
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_rest::_parse_store()
{
	pugi::xml_document doc;
	pugi::xml_parse_result const result{doc.load_buffer_inplace(buffer_.data(), buffer_.size())};
	if (result.status != pugi::xml_parse_status::status_ok)
	{
		throw std::runtime_error{result.description()};
	}
	auto node_data{doc.child("document").child("data")};
	for (; node_data && "marketdata"s != node_data.attribute("id").as_string(); node_data = node_data.next_sibling("data"))
	{}
	if (!node_data)
	{
		throw std::runtime_error{"can't find \"data\" node with id = \"marketdata\""s};
	}
	auto node_rows{node_data.child("rows")};
	if (!node_rows)
	{
		throw std::runtime_error{"can't find \"rows\" node"s};
	}
	auto node_row{node_rows.child("row")};	// parsing first row only assuming that brimary_board is requested
	if (!node_row)
	{
		throw std::runtime_error{"can't find any \"row\" node"s};
	}
	result_.data_.clear();
	result_.data_.reserve(feed_args_.size());
	_parse_date(node_row.attribute("TRADEDATE").as_string());
	for (auto const& arg : feed_args_)
	{
		result_.data_.emplace_back(node_row.attribute(arg.c_str()).as_double());
	}
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_rest::start(std::unique_ptr<keeper::data_write> dest)
{
	dest_ = std::move(dest);
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::moex_rest::read(std::span<const char> chunk)
{
	buffer_.reserve(buffer_.size() + chunk.size());
	buffer_.insert(buffer_.end(), chunk.begin(), chunk.end());
	return chunk.size();
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_rest::finish(std::span<const char> chunk)
{
	assert(dest_);
	try
	{
		read(chunk);
		_parse_store();
		dest_->add(std::make_pair(result_.date_, std::move(result_.data_)));
		dest_->finish();
	}
	catch (std::exception const& ex)
	{
		throw std::runtime_error{"failed to parse moex_rest source. "s + ex.what()};
	}
}
