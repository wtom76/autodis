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
		virtual size_t read(std::span<const char> chunk) = 0;
		virtual void finish(std::span<const char> chunk) = 0;
		virtual int last_recvd_date() = 0;
	};
}
