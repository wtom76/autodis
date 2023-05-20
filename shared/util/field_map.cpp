#include "pch.hpp"
#include "field_map.hpp"

//---------------------------------------------------------------------------------------------------------
std::vector<shared::util::field_map::name_idx> shared::util::field_map::_build_src_name_dst_idx(
	std::vector<name_name> const& src_dest_names,
	std::vector<std::string> const& dest_names) const
{
	std::vector<name_idx> src_name_dst_idx;
	src_name_dst_idx.reserve(dest_names.size());

	for (std::size_t dst_idx{0}; dst_idx != dest_names.size(); ++dst_idx)
	{
		auto const iter{std::ranges::find(src_dest_names, dest_names[dst_idx], [](auto const& pr) noexcept -> std::string const& { return pr.dst_; })};
		if (iter != src_dest_names.cend())
		{
			src_name_dst_idx.emplace_back(iter->src_, dst_idx);
		}
		else
		{
			throw std::runtime_error("requested field is unknown: \""s + dest_names[dst_idx] + '\"');
		}
	}
	return src_name_dst_idx;
}
//---------------------------------------------------------------------------------------------------------
shared::util::field_map::field_map(
	std::vector<name_name> const& src_dest_names,
	std::vector<std::string> const& dest_names,
	std::string index_src_name)
	: src_name_dst_idx_{_build_src_name_dst_idx(src_dest_names, dest_names)}
	, index_src_name_{std::move(index_src_name)}
{}
//---------------------------------------------------------------------------------------------------------
void shared::util::field_map::field_map::map_next_src_name(std::string const& src_name)
{
	if (src_name == index_src_name_)
	{
		src_idx_dst_idx_.emplace_back(idx_null_);
		index_field_src_idx_ = src_idx_dst_idx_.size();
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
