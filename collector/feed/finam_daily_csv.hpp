#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	/// class finam_daily_csv
	//---------------------------------------------------------------------------------------------------------
	class finam_daily_csv
		: public source::feed
	{
	private:
		struct row
		{
			int					date_{0};
			std::vector<double>	data_;

			row(std::size_t fields_num) : data_(fields_num, 0.) {}
		};

		char								separator_{0};
		bool								header_parsed_{false};
		shared::util::field_map				field_map_;							// maps file fields to dest fields by indexes. can contain row_idx_null_

		std::unique_ptr<keeper::data_write>	dest_;

	private:
		static char _determine_separator(auto header_begin, auto header_end) noexcept;
		std::span<const char>::iterator _parse_header(std::span<const char> chunk);
		row _read_row(std::stringstream& s);
		void _send(row&& r);
		
	public:
		finam_daily_csv(std::span<keeper::feed_args_t const> feed_args);
		//---------------------------------------------------------------------------------------------------------
		// source::feed impl		
		//---------------------------------------------------------------------------------------------------------
		void set_data_write(std::unique_ptr<keeper::data_write> dest) override;
		void start() override;
		size_t read(std::span<const char> chunk) override;
		void finish(std::span<const char> chunk) override;
		std::int64_t total_recvd_records() override { assert(false); return 0; }	// implement on demand
		//---------------------------------------------------------------------------------------------------------
		//~source::feed impl		
		//---------------------------------------------------------------------------------------------------------
	};
}