#include "pch.hpp"
#include "linear_generator.hpp"

//---------------------------------------------------------------------------------------------------------
std::size_t collector::feed::linear_generator::_index(std::vector<std::string> const& list, std::string const& value)
{
	std::size_t const idx{static_cast<std::size_t>(std::distance(list.cbegin(), std::ranges::find(list, value)))};
	if (idx >= list.size())
	{
		throw std::runtime_error("feed arguments for linear generator do not contain "s + value);
	}
	return idx;
}

//---------------------------------------------------------------------------------------------------------
collector::feed::linear_generator::linear_generator(std::vector<std::string> const& feed_args)
	: dst_idx_x_{_index(feed_args, "x"s)}
	, dst_idx_y_{_index(feed_args, "y"s)}
{
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::linear_generator::start(std::unique_ptr<keeper::data_write> dest)
{
	constexpr long long row_count_{200};
	constexpr double a_{0.02};
	constexpr double b_{0.01};

	std::vector<double> values(2, 0.);
	for (long long x{0}; x != row_count_; ++x)
	{
		values[dst_idx_x_] = x;
		values[dst_idx_y_] = a_ * x + b_;
		dest->add(std::make_pair(x, values));
	}
	dest->finish();
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::linear_generator::read(std::span<const char>)
{
	return 0;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::linear_generator::finish(std::span<const char>)
{}
