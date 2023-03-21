#pragma once

#include "base.hpp"

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class dummy
	//---------------------------------------------------------------------------------------------------------
	class dummy
		: public base
	{
	public:
		dummy(){}

		void fetch_to(feed& dest) override
		{
			dest.read({});
			dest.finish({});
		}
	};
}