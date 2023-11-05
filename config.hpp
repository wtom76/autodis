#pragma once

#include "framework.hpp"

namespace autodis
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

	config& cfg();
}