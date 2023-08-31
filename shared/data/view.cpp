#include "pch.hpp"
#include "view.hpp"

//-----------------------------------------------------------------------------------------------------
shared::data::view::series_view_t shared::data::view::series_view(frame::name_t const& name) const
{
	for (auto col_idx : col_index_)
	{
		if (frame_.name(col_idx) == name)
		{
			return series_view_t{frame_.series(col_idx), row_index_};
		}
	}
	throw std::runtime_error{"no '"s + name + "' series"s};
}
//-----------------------------------------------------------------------------------------------------
size_t shared::data::view::series_idx(frame::name_t const& name) const
{
	for (auto col_idx : col_index_)
	{
		if (frame_.name(col_idx) == name)
		{
			return col_idx;
		}
	}
	throw std::runtime_error{"no '"s + name + "' series"s};
}
//-----------------------------------------------------------------------------------------------------
shared::data::view::view(frame& frame)
	: frame_{frame}
{
	row_index_.resize(frame.index().size());
	std::iota(row_index_.begin(), row_index_.end(), 0);
	col_index_.resize(frame.col_count());
	std::iota(col_index_.begin(), col_index_.end(), 0);
}
//-----------------------------------------------------------------------------------------------------
void shared::data::view::delete_column(size_t idx) noexcept
{
	col_index_.erase(std::next(col_index_.begin(), idx));
}
//-----------------------------------------------------------------------------------------------------
bool shared::data::view::_row_has_nan(size_t view_idx) const noexcept
{
	size_t const frame_row_idx{row_index_[view_idx]};
	for (size_t frame_col_idx : col_index_)
	{
		if (std::isnan(frame_.data()[frame_col_idx][frame_row_idx]))
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
void shared::data::view::clear_lacunas() noexcept
{
	std::vector<size_t> new_row_index;
	new_row_index.reserve(row_index_.size());
	for (size_t view_idx{0}; view_idx != row_index_.size(); ++view_idx)
	{
		if (!_row_has_nan(view_idx))
		{
			new_row_index.emplace_back(row_index_[view_idx]);
		}
	}
	new_row_index.shrink_to_fit();
	row_index_ = std::move(new_row_index);
}
