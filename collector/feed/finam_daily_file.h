#pragma once

#include "../source/base.h"

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	/// class finam_daily_file
	//---------------------------------------------------------------------------------------------------------
	class finam_daily_file
		: public source::sink
	{
	private:
		static constexpr char separator_{';'};
		static constexpr size_t bulk_size_{512};

		struct field_idx
		{
			std::ptrdiff_t date_{-1};
			std::ptrdiff_t open_{-1};
			std::ptrdiff_t high_{-1};
			std::ptrdiff_t low_{-1};
			std::ptrdiff_t close_{-1};
			std::ptrdiff_t vol_{-1};
		};

		struct row
		{
			int date_{0};
			double open_{0.};
			double high_{0.};
			double low_{0.};
			double close_{0.};
			double vol_{0.};
		};

		bool				header_parsed_{false};
		field_idx			field_idx_;
		std::vector<row>	row_buffer_;

	private:
		std::span<char>::iterator _parse_header(const std::span<char> chunk);
		row _read_row(std::stringstream& s);
		void _send_rows(bool final = false);
		
	public:
		//---------------------------------------------------------------------------------------------------------
		// source::sink impl		
		//---------------------------------------------------------------------------------------------------------
		void start() override;
		size_t read(const std::span<char> chunk) override;
		void finish(const std::span<char> chunk) override;
		//---------------------------------------------------------------------------------------------------------
		//~source::sink impl		
		//---------------------------------------------------------------------------------------------------------
	};
}