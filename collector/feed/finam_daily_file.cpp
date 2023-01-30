#include "pch.h"
#include "finam_daily_file.h"

namespace collector::feed
{
	template <typename field>
	void read_field(const std::string& str, field& dest)
	{
		const std::from_chars_result res{std::from_chars(str.data(), str.data() + str.size(), dest)};
		if (res.ec != std::errc{})
		{
			throw std::runtime_error("failed to read field "s + str);
		}
	}
}
//---------------------------------------------------------------------------------------------------------
// <TICKER>;<PER>;<DATE>;<TIME>;<OPEN>;<HIGH>;<LOW>;<CLOSE>;<VOL>
std::span<char>::iterator collector::feed::finam_daily_file::_parse_header(const std::span<char> chunk)
{
	const auto line_e{std::find(chunk.begin(), chunk.end(), '\n')};
	if (line_e == chunk.end())
	{
		return chunk.begin();
	}
	auto header_e{line_e};
	while (header_e != chunk.begin() && (*std::prev(header_e) == '\r' || *std::prev(header_e) == '\n'))
	{
		--header_e;
	}
	std::stringstream s{{chunk.begin(), header_e}, std::ios_base::in};
	std::ptrdiff_t idx{0};
	std::string field;
	while (std::getline(s, field, separator_))
	{
		if (field == "<DATE>"s)
		{
			field_idx_.date_ = idx;
		}
		else if (field == "<OPEN>"s)
		{
			field_idx_.open_ = idx;
		}
		else if (field == "<HIGH>"s)
		{
			field_idx_.high_ = idx;
		}
		else if (field == "<LOW>"s)
		{
			field_idx_.low_ = idx;
		}
		else if (field == "<CLOSE>"s)
		{
			field_idx_.close_ = idx;
		}
		else if (field == "<VOL>"s)
		{
			field_idx_.vol_ = idx;
		}
		++idx;
	}
	return line_e;
}
//---------------------------------------------------------------------------------------------------------
collector::feed::finam_daily_file::row collector::feed::finam_daily_file::_read_row(std::stringstream& s)
{
	std::ptrdiff_t idx{0};
	std::string field;
	row r;
	while (std::getline(s, field, separator_))
	{
		if (idx == field_idx_.date_)
		{
			read_field(field, r.date_);
		}
		else if (idx == field_idx_.open_)
		{
			read_field(field, r.open_);
		}
		else if (idx == field_idx_.high_)
		{
			read_field(field, r.high_);
		}
		else if (idx == field_idx_.low_)
		{
			read_field(field, r.low_);
		}
		else if (idx == field_idx_.close_)
		{
			read_field(field, r.close_);
		}
		else if (idx == field_idx_.vol_)
		{
			read_field(field, r.vol_);
		}
		++idx;
	}
	return r;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_file::_send_rows(bool final)
{
	if (final || row_buffer_.size() >= bulk_size_)
	{
		row_buffer_.clear();
	}
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_file::start()
{
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::finam_daily_file::read(const std::span<char> chunk)
{
	auto next_i{chunk.begin()};
	if (!header_parsed_)
	{
		next_i = _parse_header(chunk);
		header_parsed_ = next_i != chunk.begin();
		if (!header_parsed_)
		{
			return 0;
		}
	}

	while (next_i != chunk.end())
	{
		while (next_i != chunk.end() && *next_i == '\n')
		{
			++next_i;
		}
		const auto line_e{std::find(next_i, chunk.end(), '\n')};
		if (line_e == chunk.end())
		{
			break;
		}
		std::stringstream s{{next_i, line_e}, std::ios_base::in};
		row_buffer_.emplace_back(_read_row(s));
		next_i = line_e;
	}
	return std::distance(chunk.begin(), next_i);
}
//---------------------------------------------------------------------------------------------------------
// should be last line without line end or an empty chunk
void collector::feed::finam_daily_file::finish(const std::span<char> chunk)
{
	assert(header_parsed_);

	auto next_i{chunk.begin()};
	if (next_i != chunk.end())
	{
		const auto line_e{chunk.end()};
		if (line_e == chunk.end())
		{
			return;
		}
		std::stringstream s{{next_i, line_e}, std::ios_base::in};
		row_buffer_.emplace_back(_read_row(s));
	}
}
