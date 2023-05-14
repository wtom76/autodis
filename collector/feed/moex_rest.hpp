#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	/// class moex_rest
	//---------------------------------------------------------------------------------------------------------
	class moex_rest
		: public source::feed
	{
	private:
		std::unique_ptr<keeper::data_write>	dest_;
		std::ptrdiff_t	field_idx_to_store_;		// index in row::ohlvcv_ to store to dest

	private:
	public:
		moex_rest();
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