#pragma once

#include "sink.h"

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class base
	/// base for raw bytes sources
	//---------------------------------------------------------------------------------------------------------
	class base
	{
	public:
		virtual void fetch_to(sink& dest) = 0;
	};
}
