#pragma once

#include "framework.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class partial_dependence_view
	{
	private:
		//---------------------------------------------------------------------------------------------------------
		void _draw(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names);
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit partial_dependence_view(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names, std::string caption);
	};
}