#pragma once

#include "framework.hpp"

namespace collector
{
	//---------------------------------------------------------------------------------------------------------
	/// class source_uri
	/// <type name>/<source name>
	//---------------------------------------------------------------------------------------------------------
	class source_uri
		: public shared::util::uri
	{
	public:
		using shared::util::uri::uri;

		const std::string& type_name() const noexcept
		{
			return part(0);
		}
		const std::string& name() const noexcept
		{
			return part(1);
		}
	};
}