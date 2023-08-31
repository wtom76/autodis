#pragma once

#include "framework.hpp"
#include "frame.hpp"

namespace shared::data
{
	//-----------------------------------------------------------------------------------------------------
	// class view
	//-----------------------------------------------------------------------------------------------------
	class view
	{
	// types
	private:
		using index_t = std::vector<size_t>;

	public:
		//-----------------------------------------------------------------------------------------------------
		class series_view_iterator
		{
		private:
			frame::series_t*		series_{nullptr};
			index_t::const_iterator	row_index_iter_{};

		public:
			using difference_type = std::ptrdiff_t;
			using value_type = frame::value_t;

		public:
			series_view_iterator(frame::series_t* series, index_t::const_iterator row_index_iter)
				: series_{series}
				, row_index_iter_{std::move(row_index_iter)}
			{}
			series_view_iterator()
			{}
			series_view_iterator(series_view_iterator const&) = default;
			series_view_iterator(series_view_iterator&&) = default;
			series_view_iterator& operator =(series_view_iterator const&) = default;
			series_view_iterator& operator =(series_view_iterator&&) = default;

			frame::value_t& operator*() const { return (*series_)[*row_index_iter_]; }
			frame::value_t* operator->() { return &(*series_)[*row_index_iter_]; }
			series_view_iterator& operator++() noexcept { ++row_index_iter_; return *this; }
		    series_view_iterator operator++(int) noexcept { return series_view_iterator{series_, row_index_iter_++}; }
			series_view_iterator& operator--() noexcept { --row_index_iter_; return *this; }
		    series_view_iterator operator--(int) noexcept { return series_view_iterator{series_, row_index_iter_--}; }
			friend bool operator == (series_view_iterator const& left, series_view_iterator const& right) noexcept { return left.row_index_iter_ == right.row_index_iter_; };
			friend bool operator != (series_view_iterator const& left, series_view_iterator const& right) noexcept { return left.row_index_iter_ != right.row_index_iter_; };
		};
		static_assert(std::bidirectional_iterator<series_view_iterator>);
		//-----------------------------------------------------------------------------------------------------
		class series_view_t
		{
			frame::series_t&	series_;
			index_t const&		row_index_;
		public:
			series_view_t(frame::series_t& series, index_t const& row_index)
				: series_{series}
				, row_index_{row_index}
			{}

			series_view_iterator begin() noexcept { return series_view_iterator{&series_, row_index_.begin()}; }
			series_view_iterator end() noexcept { return series_view_iterator{&series_, row_index_.end()}; }

			frame::value_t operator [](size_t idx) const noexcept { return series_[idx]; }
			frame::value_t& operator [](size_t idx) noexcept { return series_[idx]; }
		};

	// data
	private:
		frame&	frame_;
		index_t	row_index_;
		index_t	col_index_;

	// methods
	private:
		//[[nodiscard]] size_t _frame_idx(size_t view_idx) const noexcept { return row_index_[view_idx]; }
		[[nodiscard]] bool _row_has_nan(size_t view_idx) const noexcept;

	public:
		explicit view(frame& frame);

		[[nodiscard]] size_t row_count() const noexcept { return row_index_.size(); }
		[[nodiscard]] size_t col_count() const noexcept { return col_index_.size(); }
		[[nodiscard]] series_view_t series_view(size_t idx) const noexcept { return series_view_t{frame_.series(idx), row_index_}; }
		[[nodiscard]] series_view_t series_view(frame::name_t const& name) const;
		[[nodiscard]] size_t series_idx(frame::name_t const& name) const;
		// deprecated
		//[[nodiscard]] size_t frame_idx(size_t view_idx) const noexcept { return row_index_[view_idx]; }
		void delete_column(size_t idx) noexcept;
		void clear_lacunas() noexcept;
	};
}