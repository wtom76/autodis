#include "pch.hpp"
#include "rest_by_chunks.hpp"
#include "rest.hpp"

namespace
{
	//---------------------------------------------------------------------------------------------------------
	class from_arg_failed : public std::runtime_error
	{
	public:
		from_arg_failed()
			: std::runtime_error{"rest_by_chunks can't parse url. last argument should be 'from' in format 'from=yyyy-mm-dd'"s}
		{}
	};
}
//---------------------------------------------------------------------------------------------------------
collector::source::rest_by_chunks::rest_by_chunks(std::string source_args)
	: url_{std::move(source_args)}
{
}
//---------------------------------------------------------------------------------------------------------
std::pair<std::string, std::chrono::year_month_day> collector::source::rest_by_chunks::_cut_from_arg(std::string const& url)
{
	auto subrange{std::ranges::find_last(url, '&')};
	if (subrange.empty())
	{
		throw from_arg_failed{};
	}
	return {std::string{url.cbegin(), subrange.cbegin()}, _parse_from_arg({&*subrange.cbegin(), subrange.size()})};
}
//---------------------------------------------------------------------------------------------------------
std::chrono::year_month_day collector::source::rest_by_chunks::_parse_from_arg(std::span<char const> from_arg)
{
	int y{0};
	unsigned int m{0};
	unsigned int d{0};
	if (std::sscanf(from_arg.data(), "&from=%d-%u-%u", &y, &m, &d) != 3)
	{
		throw from_arg_failed{};
	}
	std::chrono::year_month_day ymd{std::chrono::year{y}, std::chrono::month(m), std::chrono::day{d}};
	if (!ymd.year().ok() || !ymd.month().ok() || !ymd.day().ok())
	{
		throw from_arg_failed{};
	}
	return ymd;
}
//---------------------------------------------------------------------------------------------------------
std::string collector::source::rest_by_chunks::_print_from_arg(std::chrono::year_month_day const& ymd)
{
	std::array<char, 32> buf;
	
	int const len{std::snprintf(buf.data(), buf.size(), "&from=%04d-%02u-%02u",
		ymd.year().operator int(),
		ymd.month().operator unsigned(),
		ymd.day().operator unsigned())};

	return {buf.data(), static_cast<std::size_t>(len)};
}
//---------------------------------------------------------------------------------------------------------
void collector::source::rest_by_chunks::fetch_to(feed& dest)
{
	std::pair<std::string, std::chrono::year_month_day> url_pr{_cut_from_arg(url_)};
	const std::chrono::year_month_day today{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
	while (url_pr.second < today)
	{
		rest src{url_pr.first + _print_from_arg(url_pr.second)};
		src.fetch_to(dest);
		std::chrono::year_month_day const last_recvd{shared::util::time::ymd_from_yyyymmdd(dest.last_recvd_date())};
		if (url_pr.second == last_recvd)
		{
			break;
		}
		url_pr.second = last_recvd;
	}
}
