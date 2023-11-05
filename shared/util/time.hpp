#pragma once

#include "framework.hpp"

namespace shared::util::time
{
	//---------------------------------------------------------------------------------------------------------
	[[nodiscard]] std::tm tm_from_yyyymmdd(uint32_t date) noexcept;
	//---------------------------------------------------------------------------------------------------------
	[[nodiscard]] uint32_t yyyymmdd(std::tm const& tm_val) noexcept;
	//---------------------------------------------------------------------------------------------------------
	[[nodiscard]] uint32_t yyyymmdd(std::chrono::year_month_day const& ymd) noexcept;
	//---------------------------------------------------------------------------------------------------------
	// parses 0000-00-00 00:00:00
	[[nodiscard]] std::tm tm_from_postgre(std::span<char const> src);
	//---------------------------------------------------------------------------------------------------------
	[[nodiscard]] std::chrono::system_clock::time_point to_time(std::tm const& tm_val);
	//---------------------------------------------------------------------------------------------------------
	[[nodiscard]] std::tm tm_date(std::chrono::system_clock::time_point tp);
}