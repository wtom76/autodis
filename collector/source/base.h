#pragma once

#include "feed.h"

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class base
	/// base for raw bytes sources
	//---------------------------------------------------------------------------------------------------------
	class base
	{
	public:
		virtual void fetch_to(feed& dest) = 0;
	};
}
