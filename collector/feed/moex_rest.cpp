#include "pch.hpp"
#include "moex_rest.hpp"
#include <pugixml.hpp>

//---------------------------------------------------------------------------------------------------------
collector::feed::moex_rest::moex_rest(std::span<keeper::feed_args_t const> feed_args)
	: feed_args_{feed_args.begin(), feed_args.end()}
{}
//---------------------------------------------------------------------------------------------------------
// 2023-10-30 23:27:11
// "2023-05-21..." -> "2023052121..." -> 20230521
// 1. MOEX puts next day in SYSTIME field at midnight even though the record represents previous day
// so if time is less than 1 hour (heuristic threshold), take previous date.
void collector::feed::moex_rest::_parse_date(std::string dt_str)
{
	if (dt_str.size() < 10)
	{
		throw std::runtime_error{"unknown SYSTIME format: \""s + dt_str + "\""};
	}

	std::tm const tm_val{shared::util::time::tm_from_postgre(dt_str)};
	std::chrono::system_clock::time_point const tp{shared::util::time::to_time(tm_val)};
	std::chrono::sys_days const sdays{std::chrono::time_point_cast<std::chrono::days>(tp)};
	std::chrono::year_month_day ymd{sdays};
	if (!ymd.ok())
	{
		throw std::runtime_error{"unknown SYSTIME format: \""s + dt_str + "\""};
	}
	// 1.
	if (std::chrono::hh_mm_ss{tp - static_cast<std::chrono::sys_days>(ymd)}.hours() <= 1h)
	{
		ymd = sdays - std::chrono::days{1};
	}
	result_.date_ = shared::util::time::yyyymmdd(ymd);
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
	pugi::xml_node node_data{doc.child("document").child("data")};
	for (; node_data && "marketdata"s != node_data.attribute("id").as_string(); node_data = node_data.next_sibling("data"))
	{}
	if (!node_data)
	{
		throw std::runtime_error{"can't find \"data\" node with id = \"marketdata\""s};
	}
	pugi::xml_node node_rows{node_data.child("rows")};
	if (!node_rows)
	{
		throw std::runtime_error{"can't find \"rows\" node"s};
	}
	result_.data_.clear();
	result_.data_.reserve(feed_args_.size());
	for (pugi::xml_node node_row{node_rows.child("row")}; node_row; node_row = node_row.next_sibling("row"))
	{
		_parse_date(node_row.attribute("SYSTIME").as_string());
		for (auto const& arg : feed_args_)
		{
			if (node_row.attribute("BOARDID").as_string() == arg.part(0))
			{
				result_.data_.emplace_back(node_row.attribute(arg.parts().back().c_str()).as_double());
			}
		}
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
		if (result_.data_.size() != feed_args_.size())
		{
			throw std::runtime_error{"number of fields found differs from requested number "s +
				std::to_string(result_.data_.size()) + " != "s + std::to_string(feed_args_.size())};
		}
		dest_->add(std::make_pair(result_.date_, std::move(result_.data_)));
		dest_->finish();
	}
	catch (std::exception const& ex)
	{
		throw std::runtime_error{"failed to parse moex_rest source. "s + ex.what()};
	}
}
