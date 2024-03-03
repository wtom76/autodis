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
		void _draw(std::vector<std::vector<double>>&& data, std::vector<std::string>&& names)
		{
			//matplot::heatmap(std::move(data))->normalization(matplot::matrix::color_normalization::columns);
			matplot::heatmap(std::move(data));

			matplot::title("model");
			matplot::axes_handle ax{matplot::gca()};
			ax->y_axis().ticklabels(std::move(names));
			xlabel(ax, "Dest");
			ylabel(ax, "Input");
			float w{ax->width()};
			ax->width(w * 0.85f);
			ax->x_origin(ax->x_origin() + w * 0.1f);

			matplot::show();
		}
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit heatmap(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names)
		{
			assert(data.size() == names.size());

			std::size_t rows_left{data.size()};
			auto data_i{std::begin(data)};
			auto names_i{std::begin(names)};
			//while (rows_left)
			{
				std::size_t cur_page_size{std::min(page_size_, rows_left)};
				rows_left -= cur_page_size;
				auto data_e{std::next(data_i, cur_page_size)};
				auto names_e{std::next(names_i, cur_page_size)};
				std::vector<std::vector<double>> page_data{data_i, data_e};
				std::vector<std::string> page_names{names_i, names_e};
				_draw(std::move(page_data), std::move(page_names));
			}
		}
	};
}