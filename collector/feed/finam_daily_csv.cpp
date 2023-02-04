#include "pch.h"
#include "finam_daily_csv.h"

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
collector::feed::finam_daily_csv::finam_daily_csv(std::string const& field_name)
{
	static const std::array<std::pair<std::string, std::ptrdiff_t>, 5> field_name_idx
	{
		std::make_pair("open"s, row_idx_open_),
		std::make_pair("high"s, row_idx_high_),
		std::make_pair("low"s, row_idx_low_),
		std::make_pair("close"s, row_idx_close_),
		std::make_pair("vol"s, row_idx_vol_)
	};
	for (auto const& fni : field_name_idx)
	{
		if (fni.first == field_name)
		{
			field_idx_to_store_ = fni.second;
			break;
		}
	}
	if (field_idx_to_store_ == row_idx_null_)
	{
		throw std::runtime_error("unknown field_name: \""s + field_name + '\"');
	}
}
//---------------------------------------------------------------------------------------------------------
// <TICKER>;<PER>;<DATE>;<TIME>;<OPEN>;<HIGH>;<LOW>;<CLOSE>;<VOL>
std::span<char>::iterator collector::feed::finam_daily_csv::_parse_header(const std::span<char> chunk)
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
	std::string field;
	while (std::getline(s, field, separator_))
	{
		if (field == "<DATE>"s)
		{
			field_map_.emplace_back(row_idx_date_);
		}
		else if (field == "<OPEN>"s)
		{
			field_map_.emplace_back(row_idx_open_);
		}
		else if (field == "<HIGH>"s)
		{
			field_map_.emplace_back(row_idx_high_);
		}
		else if (field == "<LOW>"s)
		{
			field_map_.emplace_back(row_idx_low_);
		}
		else if (field == "<CLOSE>"s)
		{
			field_map_.emplace_back(row_idx_close_);
		}
		else if (field == "<VOL>"s)
		{
			field_map_.emplace_back(row_idx_vol_);
		}
		else
		{
			field_map_.emplace_back(row_idx_null_);
		}
	}
	return line_e;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_csv::_read_row(std::stringstream& s, row& dest)
{
	std::ptrdiff_t idx{0};
	std::string field;
	while (std::getline(s, field, separator_) && idx < field_map_.size())
	{
		if (field_map_[idx] == row_idx_date_)
		{
			read_field(field, dest.date_);
		}
		else if (field_map_[idx] != row_idx_null_)
		{
			read_field(field, dest.ohlcv_[field_map_[idx]]);
		}
		++idx;
	}
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_csv::_send(row const& r)
{
	assert(dest_);
	assert(field_idx_to_store_ >= 0);

	dest_->add(std::make_pair(r.date_, r.ohlcv_[field_idx_to_store_]));
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_csv::start(std::unique_ptr<keeper::data> dest)
{
	dest_ = std::move(dest);
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::finam_daily_csv::read(const std::span<char> chunk)
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

	row r;
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
		_read_row(s, r);
		_send(r);
		next_i = line_e;
	}
	return std::distance(chunk.begin(), next_i);
}
//---------------------------------------------------------------------------------------------------------
// should be last line without line end or an empty chunk
void collector::feed::finam_daily_csv::finish(const std::span<char> chunk)
{
	assert(header_parsed_);

	row r;
	auto next_i{chunk.begin()};
	if (next_i != chunk.end())
	{
		const auto line_e{chunk.end()};
		if (line_e == chunk.end())
		{
			return;
		}
		std::stringstream s{{next_i, line_e}, std::ios_base::in};
		_read_row(s, r);
		_send(r);
	}

	dest_->finish();
	dest_.reset();
}
