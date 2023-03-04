#pragma once

#include <memory>
#include <span>
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
		virtual void start(std::unique_ptr<keeper::data_write> dest) = 0;
		virtual size_t read(const std::span<char> chunk) = 0;
		virtual void finish(const std::span<char> chunk) = 0;
	};
}
