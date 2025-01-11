#include "pch.hpp"
#include "linear_generator.hpp"

//---------------------------------------------------------------------------------------------------------
std::size_t collector::feed::linear_generator::_index(std::span<keeper::feed_args_t const> list, std::string const& value)
{
	std::size_t const idx{static_cast<std::size_t>(std::distance(list.begin(),
		std::ranges::find_if(list, [&value](auto const &args){ return args.parts().back() == value; })
	))};
	if (idx >= list.size())
	{
		throw std::runtime_error("feed arguments for linear generator do not contain "s + value);
	}
	return idx;
}

//---------------------------------------------------------------------------------------------------------
collector::feed::linear_generator::linear_generator(std::span<keeper::feed_args_t const> feed_args)
	: dst_idx_x_{_index(feed_args, "x"s)}
	, dst_idx_y_{_index(feed_args, "y"s)}
{
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::linear_generator::set_data_write(std::unique_ptr<keeper::data_write> dest)
{
	dest_ = std::move(dest);
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::linear_generator::start()
{
	assert(dest_);

	constexpr long long row_count_{200};
	constexpr double a_{0.02};
	constexpr double b_{0.01};

	std::vector<double> values(2, 0.);
	for (long long x{0}; x != row_count_; ++x)
	{
		values[dst_idx_x_] = x;
		values[dst_idx_y_] = a_ * x + b_;
		dest_->add(std::make_pair(x, values));
	}
}
//---------------------------------------------------------------------------------------------------------
size_t collector::feed::linear_generator::read(std::span<const char>)
{
	return 0;
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::linear_generator::finish(std::span<const char>)
{
	dest_->finish();
}
