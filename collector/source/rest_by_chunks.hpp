#pragma once

#include "framework.hpp"
#include "base.hpp"

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	// class rest_by_chunks
	//---------------------------------------------------------------------------------------------------------
	class rest_by_chunks
		: public base
	{
	private:
		std::string const			url_;
		std::chrono::year_month_day from_arg_;

	private:
		static std::pair<std::string, std::chrono::year_month_day> _cut_from_arg(std::string const& url);
		static std::chrono::year_month_day _parse_from_arg(std::span<char const> from_arg);
		static std::string _print_from_arg(std::chrono::year_month_day const& ymd);

	public:
		rest_by_chunks(std::string source_args);

		// base impl
		void fetch_to(feed& dest) override;
		//~base impl
	};
}