#pragma once

#include <shared/shared.hpp>

namespace collector
{
	class feed_uri
		: public shared::util::uri
	{
	public:
		using shared::util::uri::uri;

		const std::string& feed_name() const noexcept
		{
			return part(0);
		}
		const std::string& field_name() const noexcept
		{
			return part(1);
		}
	};
}