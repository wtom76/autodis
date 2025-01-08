#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	// class moex_lastday_xml
	// parsed source example: https://iss.moex.com/iss/engines/stock/markets/shares/securities/SBER.xml
	//---------------------------------------------------------------------------------------------------------
	class moex_lastday_xml
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
		std::unique_ptr<keeper::data_write>	dest_;
		std::ptrdiff_t						field_idx_to_store_;		// index in row::ohlvcv_ to store to dest
		std::vector<char>					buffer_;
		row									result_;

	private:
		void _parse_date(std::string dt_str);
		void _parse_store();

	public:
		moex_lastday_xml(std::span<keeper::feed_args_t const> feed_args);
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