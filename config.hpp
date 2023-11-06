#pragma once

#include "framework.hpp"

namespace autodis
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class config
	{
	public:
		std::string db_connection_{"postgresql://user:password@host:port/base"};
		std::string store_prediction_{"schema.procedure_namde"};

	public:
		void load();
	};

	config& cfg();
}