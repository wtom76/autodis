#include "pch.hpp"
#include "finam_daily_csv.hpp"

namespace collector::feed
{
	template <typename field>
	void read_field(std::string const& str, field& dest)
	{
		const std::from_chars_result res{std::from_chars(str.data(), str.data() + str.size(), dest)};
		if (res.ec != std::errc{})
		{
			throw std::runtime_error("failed to read field "s + str);
		}
	}
}
//---------------------------------------------------------------------------------------------------------
std::ptrdiff_t collector::feed::finam_daily_csv::_field_index(std::string const& name) noexcept
{
	static const std::array<std::string, 5> field_name_idx
	{
		"open"s,
		"high"s,
		"low"s,
		"close"s,
		"vol"s
	};
	std::ptrdiff_t idx{0};
	for (auto const& fni : field_name_idx)
	{
		if (fni == name)
		{
			return idx;
		}
		++idx;
	}
	return row_idx_null_;
}
//---------------------------------------------------------------------------------------------------------
collector::feed::finam_daily_csv::finam_daily_csv(std::string const& field_name)
	: field_idx_to_store_{_field_index(field_name)}
{
	if (field_idx_to_store_ == row_idx_null_)
	{
		throw std::runtime_error("requested field is unknown: \""s + field_name + '\"');
	}
}
//---------------------------------------------------------------------------------------------------------
char collector::feed::finam_daily_csv::_determine_separator(auto header_begin, auto header_end) noexcept
{
	static const std::string sepr{";,"s};
	const auto iter{std::find_first_of(header_begin, header_end, sepr.cbegin(), sepr.cend())};
	return iter == header_end ? '\0' : *iter;
}
//---------------------------------------------------------------------------------------------------------
// <TICKER>;<PER>;<DATE>;<TIME>;<OPEN>;<HIGH>;<LOW>;<CLOSE>;<VOL>
std::span<char>::iterator collector::feed::finam_daily_csv::_parse_header(std::span<char> const chunk)
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
	if (!(separator_ = _determine_separator(chunk.begin(), header_e)))
	{
		throw std::runtime_error("failed to determine field separator"s);
	}

	bool requested_field_found{false};
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
		if (field_map_.back() == field_idx_to_store_)
		{
			requested_field_found = true;
		}
	}
	if (!requested_field_found)
	{
		throw std::runtime_error("requested field is not found in file"s);
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
