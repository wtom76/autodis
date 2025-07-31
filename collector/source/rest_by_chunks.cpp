#include "pch.hpp"
#include "rest_by_chunks.hpp"
#include "rest.hpp"

namespace
{
	//---------------------------------------------------------------------------------------------------------
	class start_arg_failed : public std::runtime_error
	{
	public:
		start_arg_failed()
			: std::runtime_error{"rest_by_chunks can't parse url. last argument should be integer 'start'"s}
		{}
	};
}
//---------------------------------------------------------------------------------------------------------
collector::source::rest_by_chunks::rest_by_chunks(std::string source_args)
	: url_{std::move(source_args)}
{}
//---------------------------------------------------------------------------------------------------------
std::pair<std::string, std::int64_t> collector::source::rest_by_chunks::_cut_start_arg(std::string const& url)
{
	auto subrange{std::ranges::find_last(url, '&')};
	if (subrange.empty())
	{
		throw start_arg_failed{};
	}
	std::int64_t param_value{0};
	std::from_chars_result const res{std::from_chars(&*subrange.cbegin() + "&start="sv.size(), &*subrange.cbegin() + subrange.size(), param_value)};
	if (res.ec != std::errc{} || param_value < 0)
	{
		throw start_arg_failed{};
	}
	return {std::string{url.cbegin(), subrange.cbegin()}, param_value};
}
//---------------------------------------------------------------------------------------------------------
std::string collector::source::rest_by_chunks::_print_start_arg(std::int64_t start)
{
	std::array<char, 32> buf;
	int const len{std::snprintf(buf.data(), buf.size(), "&start=%ld", start)};
	return {buf.data(), static_cast<std::size_t>(len)};
}
//---------------------------------------------------------------------------------------------------------
void collector::source::rest_by_chunks::fetch_to(feed& dest)
{
	std::pair<std::string, std::int64_t> url_pr{_cut_start_arg(url_)};
	while (true)
	{
		rest src{url_pr.first + _print_start_arg(url_pr.second)};
		src.fetch_to(dest);
		std::int64_t const recvd_total{dest.total_recvd_records()};
		if (!recvd_total)
		{
			break;
		}
		url_pr.second += recvd_total;
	}
}
