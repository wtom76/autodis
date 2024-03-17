#pragma once
#include "framework.hpp"

namespace keeper
{
	class data_uri
		: public shared::util::uri
	{
	public:
		using shared::util::uri::uri;

		const std::string& table_name() const
		{
			return part(0);
		}
		const std::string& field_name() const
		{
			return part(1);
		}
	};
}