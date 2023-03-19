#pragma once
#include "framework.hpp"

namespace keeper
{
	class data_uri
		: public shared::util::uri
	{
	public:
		using shared::util::uri::uri;

		const std::string& table_name() const noexcept
		{
			return part(0);
		}
		const std::string& field_name() const noexcept
		{
			return part(1);
		}
	};
}