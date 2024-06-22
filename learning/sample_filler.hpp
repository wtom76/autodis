#pragma once

#include "framework.hpp"

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class sample_filler
	// fills input network layer with a row from source data frame view
	//-----------------------------------------------------------------------------------------------------
	class sample_filler
	{
	public:
		using data_view_t = shared::data::view;
		
	private:
		data_view_t const&						view_;
		std::vector<data_view_t::series_view_t>	src_;
		std::vector<double>*					dst_{nullptr};

	public:
		//-----------------------------------------------------------------------------------------------------
		explicit sample_filler(
			data_view_t const& dv,
			std::vector<std::string> const& col_names,
			std::vector<double>* dst)
			: view_{dv}
			, dst_{dst}
		{
			src_.reserve(col_names.size());
			for (auto const& name : col_names)
			{
				src_.emplace_back(view_.series_view(name));
			}
			assert(!dst_ || dst_->size() == src_.size());
		}
		//-----------------------------------------------------------------------------------------------------
		void set_dest(std::vector<double>& dst)
		{
			dst_ = &dst;
			assert(dst_->size() == src_.size());
		}
		//-----------------------------------------------------------------------------------------------------
		[[nodiscard]] size_t row_count() const noexcept { return view_.row_count(); }
		//-----------------------------------------------------------------------------------------------------
		[[nodiscard]] size_t col_count() const noexcept { return src_.size(); }
		//-----------------------------------------------------------------------------------------------------
		void fill(std::size_t row) const noexcept
		{
			assert(dst_);

			auto dst_i{std::begin(*dst_)};
			for (auto const& src : src_)
			{
				*dst_i++ = src[row];
			}
		}
		//-----------------------------------------------------------------------------------------------------
		void fill_last() const noexcept
		{
			fill(view_.row_count() - 1);
		}
		//-----------------------------------------------------------------------------------------------------
		// fill_to, fill_from are used in special tasks to alter input data
		//-----------------------------------------------------------------------------------------------------
		void fill_to(std::size_t row, std::vector<double>& dst_row) const noexcept
		{
			dst_row.resize(src_.size());

			auto dst_i{std::begin(dst_row)};
			for (auto const& src : src_)
			{
				*dst_i++ = src[row];
			}
		}
		//-----------------------------------------------------------------------------------------------------
		void fill_from(std::vector<double> const& src_row) const noexcept
		{
			auto dst_i{std::begin(*dst_)};
			for (auto const& src : src_row)
			{
				*dst_i++ = src;
			}
		}
	};
}