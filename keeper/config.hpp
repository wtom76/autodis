#pragma once

#include "framework.hpp"

namespace keeper
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class config
	{
	public:
		std::string db_connection_;

	public:
		void load();
	};
}