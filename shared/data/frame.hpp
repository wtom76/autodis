#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

using std::size_t;
using std::ptrdiff_t;

namespace shared::data
{
	constexpr double nan{std::numeric_limits<double>::signaling_NaN()};

	//-----------------------------------------------------------------------------------------------------
	// class frame
	//-----------------------------------------------------------------------------------------------------
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
	public:
		size_t row_count() const noexcept { return index_.size(); }
		size_t col_count() const noexcept { return data_.size(); }

		index_t& index() noexcept { return index_; }
		const index_t& index() const noexcept { return index_; }

		data_t& data() noexcept { return data_; }
		const data_t& data() const noexcept { return data_; }

		series_t& series(name_t const& name);
		const series_t& series(name_t const& name) const;

		series_t& series(std::size_t idx);
		const series_t& series(std::size_t idx) const;

		const names_t& names() const noexcept { return series_names_; }

		void clear();
		void reserve(std::size_t size);
		void resize(size_t row_count);
		
		series_t* create_series(name_t const& name, value_t initial_value = {});

		/// appends series from other if indexes are equal, throws runtime_error otherwise
		template <class other_frame>
		void append_series(other_frame&& other);

		void print_head(std::ostream& strm) const;
	};
	//-----------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------
	inline typename frame::series_t& frame::series(name_t const& name)
	{
		const auto name_i{std::find(series_names_.cbegin(), series_names_.cend(), name)};
		if (name_i == series_names_.cend())
		{
			throw std::runtime_error("No '"s + name + "' series"s);
		}
		return data_[std::distance(series_names_.cbegin(), name_i)];
	}
	//-----------------------------------------------------------------------------------------------------
	inline const typename frame::series_t& frame::series(name_t const& name) const
	{
		return const_cast<frame*>(this)->series(name);
	}
	//-----------------------------------------------------------------------------------------------------
	inline typename frame::series_t& frame::series(std::size_t idx)
	{
		return data_[idx];
	}
	//-----------------------------------------------------------------------------------------------------
	inline const typename frame::series_t& frame::series(std::size_t idx) const
	{
		return const_cast<frame*>(this)->series(idx);
	}
	//-----------------------------------------------------------------------------------------------------
	inline void frame::clear()
	{
		index_.clear();
		data_.clear();
		series_names_.clear();
	}
	//-----------------------------------------------------------------------------------------------------
	inline void frame::reserve(std::size_t size)
	{
		index_.reserve(size);
		for (series_t& col : data_)
		{
			col.reserve(size);
		}
	}
	//-----------------------------------------------------------------------------------------------------
	inline void frame::resize(size_t row_count)
	{
		index_.resize(row_count);
		for (series_t& col : data_)
		{
			col.resize(index_.size());
		}
	}
	//-----------------------------------------------------------------------------------------------------
	inline typename frame::series_t* frame::create_series(name_t const& name, value_t initial_value)
	{
		const auto name_i{std::find(series_names_.cbegin(), series_names_.cend(), name)};
		if (name_i != series_names_.cend())
		{
			assert(false);
			return nullptr;
		}
		data_.emplace_back(index_.size(), initial_value);
		series_names_.emplace_back(name);
		return &data_.back();
	}
	//-----------------------------------------------------------------------------------------------------
	template <class other_frame>
	inline void frame::append_series(other_frame&& other)
	{
		if (!std::equal(index_.cbegin(), index_.cend(),
			other.index_.cbegin(), other.index_.cend()))
		{
			throw std::runtime_error("indexes of frames are not equal"s);
		}
		data_.reserve(data_.size() + other.data_.size());
		series_names_.reserve(series_names_.size() + other.series_names_.size());
		auto data_i = other.data_.begin();
		const auto data_e = other.data_.cend();
		auto name_i = other.series_names_.begin();
		for (; data_i != data_e; ++data_i, ++name_i)
		{
			data_.emplace_back(std::forward<series_t>(*data_i));
			series_names_.emplace_back(std::forward<std::string>(*name_i));
		}
	}
	//-----------------------------------------------------------------------------------------------------
	inline void frame::print_head(std::ostream& strm) const
	{
		strm << "index";
		for (std::size_t i{0}; i != col_count(); ++i)
		{
			strm << ';' << names()[i];
		}
		if (index().empty())
		{
			strm << "\nempty\n";
			return;
		}
		strm << "\n" << index()[0];
		for (std::size_t i{0}; i != col_count(); ++i)
		{
			strm << ';' << series(i)[0];
		}
		strm << '\n';
	}
}