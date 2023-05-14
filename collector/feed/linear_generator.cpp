#include "pch.hpp"
#include "linear_generator.hpp"

//---------------------------------------------------------------------------------------------------------
collector::feed::linear_generator::linear_generator(std::string const& field_name)
	: field_idx_to_store_{field_name == "x" ? 0 : (field_name == "y" ? 1 : -1)}
{
	if (field_idx_to_store_ == -1)
	{
		throw std::runtime_error("requested field is unknown: \""s + field_name + '\"');
	}
}
//---------------------------------------------------------------------------------------------------------
void collector::feed::linear_generator::start(std::unique_ptr<keeper::data_write> dest)
{
	dest_ = std::move(dest);

	data_.clear();
	data_.reserve(row_count_);
	for (long long idx{0}; idx != row_count_; ++idx)
	{
		data_.emplace_back(row_t{static_cast<double>(idx), a_ * idx + b_});
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
	for (long long idx{0}; idx != row_count_; ++idx)
	{
		dest_->add(std::make_pair(idx, data_[idx][field_idx_to_store_]));
	}

	dest_->finish();
	dest_.reset();
}
