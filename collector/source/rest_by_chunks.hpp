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
		static std::pair<std::string, std::int64_t> _cut_start_arg(std::string const& url);
		static std::string _print_start_arg(std::int64_t start);

	public:
		rest_by_chunks(std::string source_args);

		// base impl
		void fetch_to(feed& dest) override;
		//~base impl
	};
}