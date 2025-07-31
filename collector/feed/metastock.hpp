#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed::metastock
{
	//---------------------------------------------------------------------------------------------------------
	// class metastock
	//---------------------------------------------------------------------------------------------------------
	class metastock
		: public source::feed
	{
		shared::util::field_map				field_map_;							// maps file fields to dest fields by indexes. can contain row_idx_null_
		std::unique_ptr<keeper::data_write>	dest_;

	public:
		metastock(std::span<keeper::feed_args_t const> feed_args);
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