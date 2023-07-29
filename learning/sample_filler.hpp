#pragma once

#include "framework.hpp"

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class sample_filler
	//-----------------------------------------------------------------------------------------------------
	class sample_filler
	{
	public:
		using data_view_t = shared::data::view;
		
	private:
		data_view_t const& view_;
		std::vector<data_view_t::series_view_t> src_;

	public:
		//-----------------------------------------------------------------------------------------------------
		explicit sample_filler(
			data_view_t const& dv,
			std::vector<std::string> const& col_names)
			: view_{dv}
		{
			src_.reserve(col_names.size());
			for (auto const& name : col_names)
			{
				src_.emplace_back(view_.series_view(name));
			}
		}
		//-----------------------------------------------------------------------------------------------------
		[[nodiscard]] size_t row_count() const noexcept { return view_.row_count(); }
		//-----------------------------------------------------------------------------------------------------
		[[nodiscard]] size_t col_count() const noexcept { return src_.size(); }
		//-----------------------------------------------------------------------------------------------------
		void fill(std::ptrdiff_t row, std::vector<double>& dst) const noexcept
		{
			assert(dst.size() == src_.size());

			auto dst_i{std::begin(dst)};
			for (auto const& src : src_)
			{
				++*dst_i = src[row];
			}
		}
	};
}