#pragma once

#include "framework.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class heatmap
	{
	private:
		static constexpr std::size_t page_size_{32};
	
	private:
		//---------------------------------------------------------------------------------------------------------
		std::vector<std::size_t> _select_by_minmax(std::vector<std::vector<double>> const& data) const;
		//---------------------------------------------------------------------------------------------------------
		void _draw(std::vector<std::vector<double>>&& data, std::vector<std::string>&& names, std::string caption);
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit heatmap(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names, std::string caption);
	};
}