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
collector::feed::finam_daily_csv::finam_daily_csv(std::span<keeper::feed_args_t const> feed_args)
	: field_map_{
		{
			{"<OPEN>"s, "open"s},
			{"<HIGH>"s, "high"s},
			{"<LOW>"s, "low"s},
			{"<CLOSE>"s, "close"s},
			{"<VOL>"s, "vol"s}
		},
		feed_args | std::views::transform([](keeper::feed_args_t const& args){ return args.parts().back(); }),
		"<DATE>"s}
{}
//---------------------------------------------------------------------------------------------------------
char collector::feed::finam_daily_csv::_determine_separator(auto header_begin, auto header_end) noexcept
{
	static const std::string sepr{";,"s};
	const auto iter{std::find_first_of(header_begin, header_end, sepr.cbegin(), sepr.cend())};
	return iter == header_end ? '\0' : *iter;
}
//---------------------------------------------------------------------------------------------------------
// <TICKER>;<PER>;<DATE>;<TIME>;<OPEN>;<HIGH>;<LOW>;<CLOSE>;<VOL>
std::span<const char>::iterator collector::feed::finam_daily_csv::_parse_header(std::span<const char> chunk)
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

	std::stringstream s{{chunk.begin(), header_e}, std::ios_base::in};
	std::string field;
	while (std::getline(s, field, separator_))
	{
		field_map_.map_next_src_name(field);
	}
	field_map_.validate();
	return line_e;
}
//---------------------------------------------------------------------------------------------------------
collector::feed::finam_daily_csv::row collector::feed::finam_daily_csv::_read_row(std::stringstream& s)
{
	std::size_t src_idx{0};
	std::string field;
	row result{field_map_.field_num()};
	while (std::getline(s, field, separator_))
	{
		if (src_idx == field_map_.index_field_src_idx())
		{
			read_field(field, result.date_);
		}
		else if (field_map_.dst_idx(src_idx) != field_map_.null())
		{
			read_field(field, result.data_[field_map_.dst_idx(src_idx)]);
		}
		else if (src_idx >= result.data_.size())
		{
			throw std::runtime_error("line in file has more fields than header"s);
		}
		++src_idx;
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_csv::_send(row&& r)
{
	assert(dest_);

	dest_->add(std::make_pair(r.date_, std::move(r.data_)));
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::finam_daily_csv::start(std::unique_ptr<keeper::data_write> dest)
{
	dest_ = std::move(dest);
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::finam_daily_csv::read(std::span<const char> chunk)
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
		_send(_read_row(s));
		next_i = line_e;
	}
	return std::distance(chunk.begin(), next_i);
}
//---------------------------------------------------------------------------------------------------------
// should be last line without line end or an empty chunk
void collector::feed::finam_daily_csv::finish(std::span<const char> chunk)
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
		_send(_read_row(s));
	}

	dest_->finish();
	dest_.reset();
}
