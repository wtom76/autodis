#include "pch.hpp"
#include "box_plot.hpp"

//---------------------------------------------------------------------------------------------------------
void autodis::visual::box_plot::_draw(std::vector<double> const& data, std::vector<std::string> const& names)
{
    matplot::boxplot(data, names);
    matplot::xlabel("series");
    matplot::ylabel("scatter");
}
//---------------------------------------------------------------------------------------------------------
autodis::visual::box_plot::box_plot(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names, std::string caption)
{
	assert(data.size() == names.size());
	matplot::title(std::move(caption));
	std::vector<double> flat_data;
	std::vector<std::string> flat_names;
	auto name_i{std::begin(names)};
	for (std::vector<double> const& cell : data)
	{
		for (double val : cell)
		{
			flat_data.emplace_back(val);
			flat_names.emplace_back(*name_i);
		}
		++name_i;
	}
	if (!flat_data.empty())
	{
		_draw(flat_data, flat_names);
	}
	matplot::show();
}
