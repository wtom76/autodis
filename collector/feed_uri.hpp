#pragma once

#include "framework.hpp"

namespace collector
{
	//---------------------------------------------------------------------------------------------------------
	/// class feed_uri
	/// <feed name>/<field name>
	//---------------------------------------------------------------------------------------------------------
	class feed_uri
		: public shared::util::uri
	{
	public:
		using shared::util::uri::uri;

		const std::string& feed_name() const noexcept
		{
			return part(0);
		}
	};
}