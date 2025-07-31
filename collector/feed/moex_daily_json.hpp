#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	// class moex_daily_json
	// parsed source example: https://iss.moex.com/iss/engines/stock/markets/shares/boards/tqbr/securities/sber/candles.json?from=2025-01-01&till=2025-01-08&interval=24
	//---------------------------------------------------------------------------------------------------------
	class moex_daily_json
		: public source::feed
	{
	private:
		struct row
		{
			int					date_{0};
			std::vector<double>	data_;

			row() = default;
			row(std::size_t fields_num) : data_(fields_num, 0.) {}
		};

	private:
		std::vector<keeper::feed_args_t>	feed_args_;
		shared::util::field_map				field_map_;					// maps file fields to dest fields by indexes. can contain row_idx_null_
		std::unique_ptr<keeper::data_write>	dest_;
		std::vector<char>					buffer_;
		std::int64_t						total_recvd_records_{0};

	private:
		int _parse_date(std::string const& dt_str);
		void _parse_store();
		row _read_row(nlohmann::json const& row_j);
		void _send(row&& r);

	public:
		moex_daily_json(std::span<keeper::feed_args_t const> feed_args);
		//---------------------------------------------------------------------------------------------------------
		// source::feed impl		
		//---------------------------------------------------------------------------------------------------------
		void set_data_write(std::unique_ptr<keeper::data_write> dest) override;
		void start() override;
		size_t read(std::span<const char> chunk) override;
		void finish(std::span<const char> chunk) override;
		std::int64_t total_recvd_records() override { return total_recvd_records_; }
		//---------------------------------------------------------------------------------------------------------
		//~source::feed impl		
		//---------------------------------------------------------------------------------------------------------
	};
}