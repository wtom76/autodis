#pragma once

#include "framework.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class box_plot
	{
	private:
		//---------------------------------------------------------------------------------------------------------
		void _draw(std::vector<double> const& data, std::vector<std::string> const& names);
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit box_plot(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names, std::string caption);
	};
}