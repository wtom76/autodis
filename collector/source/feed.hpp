#pragma once

#include "framework.hpp"
#include <keeper/keeper.hpp>

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class feed
	//---------------------------------------------------------------------------------------------------------
	class feed
	{
	public:
		virtual ~feed(){}
		virtual void set_data_write(std::unique_ptr<keeper::data_write> dest) = 0;
		virtual void start() = 0;
		// returns bytes read from chunk
		virtual size_t read(std::span<const char> chunk) = 0;
		virtual void finish(std::span<const char> chunk) = 0;
		// count of records received from start 
		virtual std::int64_t total_recvd_records() = 0;
	};
}
