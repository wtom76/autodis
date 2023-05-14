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
		static constexpr std::ptrdiff_t row_idx_null_{-1};
		static constexpr std::ptrdiff_t row_idx_date_{std::numeric_limits<std::ptrdiff_t>::max()};
		static constexpr std::ptrdiff_t row_idx_open_{0};
		static constexpr std::ptrdiff_t row_idx_high_{1};
		static constexpr std::ptrdiff_t row_idx_low_{2};
		static constexpr std::ptrdiff_t row_idx_close_{3};
		static constexpr std::ptrdiff_t row_idx_vol_{4};

		// field_map_t[idx in source] == idx in dest row
		// -1 idx in dest row is for date
		using field_map_t = std::vector<std::ptrdiff_t>;

		struct row
		{
			int						date_{0};
			std::array<double, 5>	ohlcv_{0.};	// open, high, low, clode, vol
		};

		char			separator_{0};
		bool			header_parsed_{false};
		field_map_t		field_map_;

		std::unique_ptr<keeper::data_write>	dest_;
		std::ptrdiff_t	field_idx_to_store_; // index in row::ohlvcv_ to store to dest

	private:
		static std::ptrdiff_t _field_index(std::string const& name) noexcept;
		static char _determine_separator(auto header_begin, auto header_end) noexcept;
		std::span<const char>::iterator _parse_header(std::span<const char> chunk);
		void _read_row(std::stringstream& s, row& dest);
		void _send(row const& r);
		
	public:
		finam_daily_csv(std::string const& field_name);
		//---------------------------------------------------------------------------------------------------------
		// source::feed impl		
		//---------------------------------------------------------------------------------------------------------
		void start(std::unique_ptr<keeper::data_write> dest) override;
		size_t read(std::span<const char> chunk) override;
		void finish(std::span<const char> chunk) override;
		//---------------------------------------------------------------------------------------------------------
		//~source::feed impl		
		//---------------------------------------------------------------------------------------------------------
	};
}