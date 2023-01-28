#pragma once

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	/// class base
	/// base for feeds
	/// feeds parse and push data to keeper
	//---------------------------------------------------------------------------------------------------------
	class base
	{
	public:
		virtual void fetch() = 0;
	};
}
