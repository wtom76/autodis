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
		std::string feature_table_name_;

	public:
		void load();
	};
}