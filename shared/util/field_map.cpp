#include "pch.hpp"
#include "field_map.hpp"

//---------------------------------------------------------------------------------------------------------
void shared::util::field_map::field_map::clear()
{
	src_idx_dst_idx_.clear();
	index_field_src_idx_ = idx_null_;
}
//---------------------------------------------------------------------------------------------------------
void shared::util::field_map::field_map::map_next_src_name(std::string const& src_name)
{
	if (src_name == index_src_name_)
	{
		index_field_src_idx_ = src_idx_dst_idx_.size();
		src_idx_dst_idx_.emplace_back(idx_null_);
		return;
	}
	auto const iter{std::ranges::find(src_name_dst_idx_, src_name, [](auto const& pr) noexcept -> std::string const& { return pr.src_; })};
	src_idx_dst_idx_.emplace_back(iter != src_name_dst_idx_.cend() ? iter->idx_ : idx_null_);
}
//---------------------------------------------------------------------------------------------------------
void shared::util::field_map::field_map::validate() const
{
	if (index_field_src_idx_ == idx_null_)
	{
		throw std::runtime_error("index field "s + index_src_name_ + " isn't found in source"s);
	}
	if (src_idx_dst_idx_.size() - std::ranges::count(src_idx_dst_idx_, idx_null_) != src_name_dst_idx_.size())
	{
		throw std::runtime_error("not all requested fields are found in source"s);
	}
}
//---------------------------------------------------------------------------------------------------------
std::size_t shared::util::field_map::field_map::dst_idx(std::size_t src_idx) const noexcept
{
	return src_idx < src_idx_dst_idx_.size() ? src_idx_dst_idx_[src_idx] : idx_null_;
}
