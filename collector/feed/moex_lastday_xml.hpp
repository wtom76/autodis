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
		void set_data_write(std::unique_ptr<keeper::data_write> dest) override;
		void start() override;
		size_t read(std::span<const char> chunk) override;
		void finish(std::span<const char> chunk) override;
		int last_recvd_date() override { assert(false); return 0; }	// implement on demand
		//---------------------------------------------------------------------------------------------------------
		//~source::feed impl		
		//---------------------------------------------------------------------------------------------------------
	};
}