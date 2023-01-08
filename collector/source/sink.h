#pragma once

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class sink
	//---------------------------------------------------------------------------------------------------------
	class sink
	{
	public:
		virtual void start() = 0;
		virtual size_t read(const std::span<char> chunk) = 0;
		virtual void finish(const std::span<char> chunk) = 0;
	};
}
