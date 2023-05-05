#pragma once
#include "framework.hpp"

namespace shared::data
{
	constexpr double nan{std::numeric_limits<double>::signaling_NaN()};

	//---------------------------------------------------------------------------------------------------------
	// class frame
	//---------------------------------------------------------------------------------------------------------
	class frame
	{
	// types
	public:
		using index_value_t	= long long;
		using value_t		= double;
		using name_t		= std::string;
		using index_t		= std::vector<index_value_t>;
		using series_t		= std::vector<value_t>;
		using data_t		= std::vector<series_t>;
		using names_t		= std::vector<name_t>;

	// data
	private:
		index_t		index_;
		data_t		data_;
		names_t		series_names_;

	// methods
	private:
		void _shrink_to_fit();
		template <class other_frame>
		void _append_series(other_frame&& other, std::vector<std::size_t> const& series_idxs);
		void _left_join(frame const& other, std::size_t series_idx);
		void _left_join(frame const& other, std::vector<std::size_t> const& series_idxs = {});
		void _outer_join(frame const& other, std::vector<std::size_t> const& series_idxs = {});
	public:
		size_t row_count() const noexcept { return index_.size(); }
		size_t col_count() const noexcept { return data_.size(); }

		index_t& index() noexcept { return index_; }
		index_t const& index() const noexcept { return index_; }

		data_t& data() noexcept { return data_; }
		data_t const& data() const noexcept { return data_; }

		series_t& series(name_t const& name);
		series_t const& series(name_t const& name) const;

		series_t& series(std::size_t idx);
		series_t const& series(std::size_t idx) const;

		names_t& names() noexcept { return series_names_; }
		names_t const& names() const noexcept { return series_names_; }

		name_t& name(std::size_t idx) noexcept { return series_names_[idx]; }
		name_t const& name(std::size_t idx) const noexcept { return series_names_[idx]; }

		void clear() noexcept;
		void reserve(std::size_t size);
		void resize(size_t row_count);
		void swap(frame& other) noexcept;
		
		series_t* create_series(name_t const& name, value_t initial_value = nan);

		template <class other_frame>
		void append_series(other_frame&& other, std::vector<std::size_t> const& series_idxs = {});		// appends series from other if indexes are equal, throws runtime_error otherwise
		template <class other_frame>
		void left_join(other_frame&& other, std::vector<std::size_t> const& series_idxs = {});
		template <class other_frame>
		void outer_join(other_frame&& other, std::vector<std::size_t> const& series_idxs = {});

		void delete_series(std::size_t idx) noexcept;

		frame clear_lacunas() const;																	// returns frame without rows containing nan

		void print(std::ostream& strm) const;
		void print_head(std::ostream& strm) const;
		void print_shape(std::ostream& strm) const;
	};
	//-----------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------
	template <class other_frame>
	inline void frame::_append_series(other_frame&& other, std::vector<std::size_t> const& series_idxs)
	{
		assert(index_.empty() || std::equal(index_.cbegin(), index_.cend(), other.index_.cbegin(), other.index_.cend()));

		if (index_.empty())
		{
			index_ = std::forward<typename other_frame::index_t>(other.index_);
			index_.shrink_to_fit();
		}

		if (series_idxs.empty())
		{
			data_.reserve(data_.size() + other.data_.size());
			series_names_.reserve(series_names_.size() + other.series_names_.size());

			auto data_i{other.data_.begin()};
			auto const data_e{other.data_.cend()};
			auto name_i{other.series_names_.begin()};
			for (; data_i != data_e; ++data_i, ++name_i)
			{
				data_.emplace_back(std::forward<typename other_frame::series_t>(*data_i));
				series_names_.emplace_back(std::forward<typename other_frame::name_t>(*name_i));
			}
		}
		else
		{
			data_.reserve(data_.size() + series_idxs.size());
			series_names_.reserve(series_names_.size() + series_idxs.size());
			for (std::size_t idx_idx{0}; idx_idx < series_idxs.size(); ++idx_idx)
			{
				data_.emplace_back(std::forward<typename other_frame::series_t>(other.series(series_idxs[idx_idx])));
				series_names_.emplace_back(std::forward<typename other_frame::name_t>(other.name(series_idxs[idx_idx])));
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------
	// appends series from other if indexes are equal, throws runtime_error otherwise
	template <class other_frame>
	inline void frame::append_series(other_frame&& other, std::vector<std::size_t> const& series_idxs)
	{
		if (!index_.empty() && !std::equal(index_.cbegin(), index_.cend(), other.index_.cbegin(), other.index_.cend()))
		{
			throw std::runtime_error("indexes of frames are not equal"s);
		}
		_append_series(std::forward<other_frame>(other), series_idxs);
	}
	//-----------------------------------------------------------------------------------------------------
	template <class other_frame>
	inline void frame::left_join(other_frame&& other, std::vector<std::size_t> const& series_idxs)
	{
		if (index_.empty() || std::equal(index_.cbegin(), index_.cend(), other.index_.cbegin(), other.index_.cend()))
		{
			_append_series(std::forward<other_frame>(other), series_idxs);
			return;
		}
		_left_join(other, series_idxs);
	}
	//-----------------------------------------------------------------------------------------------------
	// indexes should be sorted in ascending order
	template <class other_frame>
	inline void frame::outer_join(other_frame&& other, std::vector<std::size_t> const& series_idxs)
	{
		if (index_.empty() || std::equal(index_.cbegin(), index_.cend(), other.index_.cbegin(), other.index_.cend()))
		{
			_append_series(std::forward<other_frame>(other), series_idxs);
			return;
		}
		_outer_join(other, series_idxs);
	}
}