#include "pch.hpp"
#include "moex_intra_json.hpp"

//---------------------------------------------------------------------------------------------------------
collector::feed::moex_intra_json::moex_intra_json(std::span<keeper::feed_args_t const> feed_args)
	: feed_args_{feed_args.begin(), feed_args.end()}
	, field_map_{
		{
			{"open"s,	"open"s},
			{"high"s,	"high"s},
			{"low"s,	"low"s},
			{"close"s,	"close"s},
			{"value"s,	"value"s},
			{"volume"s,	"volume"s}
		},
		feed_args | std::views::transform([](keeper::feed_args_t const& args){ return args.parts().back(); }),
		"begin"s}
{}
//---------------------------------------------------------------------------------------------------------
// 2023-10-30 23:27:11
// "2023-05-21..." -> "2023052121..." -> 2023052107 (yyyymmddhh)
std::int64_t collector::feed::moex_intra_json::_parse_date(std::string const& dt_str)
{
	if (dt_str.size() < 10)
	{
		throw std::runtime_error{"unknown SYSTIME format: \""s + dt_str + "\""};
	}

	std::tm const tm_val{shared::util::time::tm_from_postgre(dt_str)};
	return shared::util::time::yyyymmddhh(tm_val);
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_intra_json::_parse_store()
{
	nlohmann::json const candles_j = nlohmann::json::parse(buffer_).at("candles");		// don't use {} here istead of =. https://github.com/nlohmann/json/issues/2204

	std::vector<std::string> columns;
	candles_j.at("columns"s).get_to(columns);
	std::ranges::for_each(columns, [this](std::string const& field){ field_map_.map_next_src_name(field); });
	field_map_.validate();

	nlohmann::json const data_j = candles_j.at("data"s);
	for (auto const& r_j : data_j)
	{
		_send(_read_row(r_j));
	}
}
//---------------------------------------------------------------------------------------------------------
collector::feed::moex_intra_json::row collector::feed::moex_intra_json::_read_row(nlohmann::json const& row_j)
{
	std::size_t filled_num{0};
	std::size_t src_idx{0};
	row result{field_map_.field_num()};
	for (auto const& field_j : row_j)
	{
		if (src_idx == field_map_.index_field_src_idx())
		{
			result.date_ = _parse_date(field_j.get<std::string>());
			if (result.date_ < 1900000000 || result.date_ > 3000000000)
			{
				throw std::runtime_error("can't parse date part"s);
			}
			if (last_recvd_date_ < result.date_)
			{
				last_recvd_date_ = result.date_;
			}
			++filled_num;
		}
		else if (field_map_.dst_idx(src_idx) != field_map_.null())
		{
			field_j.get_to(result.data_[field_map_.dst_idx(src_idx)]);
			++filled_num;
		}
		++src_idx;
	}
	if (filled_num < result.data_.size() + 1)
	{
		throw std::runtime_error("not all fields found in data row"s);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_intra_json::_send(row&& r)
{
	assert(dest_);

	dest_->add(std::make_pair(r.date_, std::move(r.data_)));
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_intra_json::set_data_write(std::unique_ptr<keeper::data_write> dest)
{
	dest_ = std::move(dest);
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_intra_json::start()
{
	assert(dest_);
	assert(buffer_.empty());
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::moex_intra_json::read(std::span<const char> chunk)
{
	buffer_.reserve(buffer_.size() + chunk.size());
	buffer_.insert(buffer_.end(), chunk.begin(), chunk.end());
	return chunk.size();
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::moex_intra_json::finish(std::span<const char> chunk)
{
	assert(dest_);
	try
	{
		read(chunk);
		_parse_store();
		dest_->finish();
		buffer_.clear();
		field_map_.clear();
	}
	catch (std::exception const& ex)
	{
		{
			std::ofstream f{"feed_dump.json"};
			f.write(buffer_.data(), buffer_.size());
		}
		throw std::runtime_error{"failed to parse moex_intra_json source. see feed_dump.json. "s + ex.what()};
	}
}
