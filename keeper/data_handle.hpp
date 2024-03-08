#pragma once

#include "framework.hpp"

namespace keeper
{
	//---------------------------------------------------------------------------------------------------------
	// data_handle
	//---------------------------------------------------------------------------------------------------------
	class data_handle
	{
	private:
		shared::data::frame df_;
		// norm
	public:
		data_handle();
	};
}