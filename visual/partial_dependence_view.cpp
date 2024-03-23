#include "pch.hpp"
#include "partial_dependence_view.hpp"

//---------------------------------------------------------------------------------------------------------
void autodis::visual::partial_dependence_view::_draw(std::vector<std::vector<double>> const& data, std::vector<std::string> const&)
{
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> c;
	x.reserve(data.size() * data.front().size());
	y.reserve(data.size() * data.front().size());
	c.reserve(data.size() * data.front().size());
	double x_counter{0.};
	for (std::vector<double> const& scatter : data)
	{
		x_counter += 1.;
		for (double point : scatter)
		{
			x.push_back(x_counter);
			y.push_back(point);
			c.push_back(x_counter);
		}
	}
	matplot::scatter(x, y, 1., c);
}
//---------------------------------------------------------------------------------------------------------
autodis::visual::partial_dependence_view::partial_dependence_view(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names, std::string caption)
{
	assert(data.size() == names.size());
	matplot::title(std::move(caption));
	if (!data.empty())
	{
		_draw(data, names);
	}
	matplot::show();
}
