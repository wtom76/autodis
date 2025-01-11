#include "pch.hpp"
#include "time.hpp"
#include <time.h>

//---------------------------------------------------------------------------------------------------------
std::tm shared::util::time::tm_from_yyyymmdd(uint32_t yyyymmdd) noexcept
{
	std::tm tm_val{};
	tm_val.tm_mday = yyyymmdd % 100;
	yyyymmdd /= 100;
	tm_val.tm_mon = yyyymmdd % 100 - 1;
	yyyymmdd /= 100;
	tm_val.tm_year = yyyymmdd - 1900;
	return tm_val;
}
//---------------------------------------------------------------------------------------------------------
uint32_t shared::util::time::yyyymmdd(std::tm const& tm_val) noexcept
{
	return (tm_val.tm_year + 1900) * 10000 + (tm_val.tm_mon + 1) * 100 + tm_val.tm_mday;
}
//---------------------------------------------------------------------------------------------------------
uint32_t shared::util::time::yyyymmdd(std::chrono::year_month_day const& ymd) noexcept
{
	return static_cast<int>(ymd.year()) * 10000 + static_cast<unsigned int>(ymd.month()) * 100 + static_cast<unsigned int>(ymd.day());
}
//---------------------------------------------------------------------------------------------------------
std::chrono::year_month_day shared::util::time::ymd_from_yyyymmdd(uint32_t yyyymmdd) noexcept
{
	unsigned int const d{yyyymmdd % 100};
	yyyymmdd /= 100;
	unsigned int const m{yyyymmdd % 100};
	yyyymmdd /= 100;
	return {std::chrono::year{static_cast<int>(yyyymmdd)}, std::chrono::month{m}, std::chrono::day{d}};
}
//---------------------------------------------------------------------------------------------------------
// parses 0000-00-00 00:00:00
std::tm shared::util::time::tm_from_postgre(std::span<char const> src)
{
	std::tm tm_val{};
	if (6 != std::sscanf(src.data(), "%04d-%02d-%02d %02d:%02d:%02d",
		&tm_val.tm_year,
		&tm_val.tm_mon,
		&tm_val.tm_mday,
		&tm_val.tm_hour,
		&tm_val.tm_min,
		&tm_val.tm_sec))
	{
		throw std::invalid_argument("time_from_postgre failed with '"s + src.data() + '\'');
	}
	--tm_val.tm_mon;
	tm_val.tm_year -= 1900;
	return tm_val;
}
//---------------------------------------------------------------------------------------------------------
std::chrono::system_clock::time_point shared::util::time::to_time(std::tm const& tm_val)
{
	return std::chrono::system_clock::from_time_t(timegm(const_cast<std::tm*>(&tm_val)));
}
//---------------------------------------------------------------------------------------------------------
std::tm shared::util::time::tm_date(std::chrono::system_clock::time_point tp)
{
	std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(tp)};
	std::tm tm_val{};
	tm_val.tm_year = static_cast<int>(ymd.year()) - 1900;
	tm_val.tm_mon = static_cast<unsigned int>(ymd.month()) - 1;
	tm_val.tm_mday = static_cast<unsigned int>(ymd.day());
	return tm_val;
}
