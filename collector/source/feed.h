#pragma once
#include <keeper/keeper.h>

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class feed
	//---------------------------------------------------------------------------------------------------------
	class feed
	{
	public:
		virtual ~feed(){}
		virtual void start(std::unique_ptr<keeper::data> dest) = 0;
		virtual size_t read(const std::span<char> chunk) = 0;
		virtual void finish(const std::span<char> chunk) = 0;
	};
}
