#include "pch.hpp"
#include "heatmap.hpp"

//---------------------------------------------------------------------------------------------------------
std::vector<std::size_t> autodis::visual::heatmap::_sorted_page(std::vector<std::vector<double>> const& data) const
{
	std::vector<double> extremum(data.size());
	std::ranges::transform(data, std::begin(extremum),
		[](std::vector<double> const& line) constexpr
		{
			auto const minmax{std::ranges::minmax(line)};
			return std::max(std::abs(minmax.max), std::abs(minmax.min));
		});
	std::vector<std::size_t> indexes(data.size());
	std::iota(std::begin(indexes), std::end(indexes), 0);
	auto const middle_i{std::next(std::begin(indexes), std::min(page_size_, indexes.size()))};
	std::ranges::partial_sort(indexes, middle_i,
		[&extremum](std::size_t left, std::size_t right) constexpr
		{
			 return extremum[left] > extremum[right];
		});
	return { std::begin(indexes), middle_i };
}
//---------------------------------------------------------------------------------------------------------
void autodis::visual::heatmap::_draw(std::vector<std::vector<double>>&& data, std::vector<std::string>&& names, std::string caption)
{
	matplot::figure_handle fh{matplot::figure()};
	matplot::axes_handle ax{fh->current_axes()};
	matplot::heatmap(ax, std::move(data));
	std::ranges::replace(caption, '_', ' ');
	matplot::title(ax, std::move(caption));
	ax->y_axis().ticklabels(std::move(names));
	xlabel(ax, "dest");
	ylabel(ax, "input");
	float w{ax->width()};
	ax->width(w * 0.85f);
	ax->x_origin(ax->x_origin() + w * 0.1f);
	matplot::show(fh);
}
//---------------------------------------------------------------------------------------------------------
autodis::visual::heatmap::heatmap(std::vector<std::vector<double>> const& data, std::vector<std::string> const& names, std::string caption)
{
	assert(data.size() == names.size());

	std::vector<std::size_t> indexes{_sorted_page(data)};
	std::vector<std::vector<double>> page_data;
	std::vector<std::string> page_names;
	page_data.reserve(indexes.size());
	page_names.reserve(indexes.size());
	for (std::size_t i : indexes)
	{
		page_data.emplace_back(data[i]);
		page_names.emplace_back(names[i]);
		std::ranges::replace(page_names.back(), '_', ' ');
	}
	_draw(std::move(page_data), std::move(page_names), std::move(caption));
}
