#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

#include "frame.hpp"

namespace shared::data
{
	//-----------------------------------------------------------------------------------------------------
	// class view
	//-----------------------------------------------------------------------------------------------------
	class view
	{
	// types
	public:

	// data
	private:
		frame&					frame_;
		std::vector<size_t>		index_;

	// methods
	public:
		explicit view(frame& frame);

		size_t row_count() const noexcept { return index_.size(); }
		frame const& data() const noexcept { return frame_; }
		size_t frame_index(size_t view_index) const noexcept;
		//bool row_has_nan(size_t row_idx) const noexcept;
		//void delete_rows_with_nan() noexcept;
	};
	//-----------------------------------------------------------------------------------------------------
	inline view::view(frame& frame)
		: frame_{frame}
	{
		index_.resize(frame.index().size());
		std::iota(index_.begin(), index_.end(), 0);
	}
	//-----------------------------------------------------------------------------------------------------
	inline size_t view::frame_index(size_t view_index) const noexcept
	{
		return index_[view_index];
	}
	//-----------------------------------------------------------------------------------------------------
	//inline bool view::row_has_nan(size_t row_idx) const noexcept
	//{
	//	for (const auto& series : frame_.data())
	//	{
	//		if (std::isnan(series[index_[row_idx]]))
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//}
	//-----------------------------------------------------------------------------------------------------
	//inline void view::delete_rows_with_nan() noexcept
	//{
	//	std::vector<size_t> new_index;
	//	new_index.reserve(index_.size());
	//	for (size_t i = 0; i != index_.size(); ++i)
	//	{
	//		if (!row_has_nan(i))
	//		{
	//			new_index.emplace_back(index_[i]);
	//		}
	//	}
	//	index_ = std::move(new_index);
	//}
}