#include "pch.hpp"
#include "frame.hpp"

//-----------------------------------------------------------------------------------------------------
void shared::data::frame::_shrink_to_fit()
{
	index_.shrink_to_fit();
	for (auto& col : data_)
	{
		col.shrink_to_fit();
	}
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::_left_join(frame const& other, std::size_t series_idx)
{
	auto dst_i{create_series(other.name(series_idx))->begin()};
	
	auto index_i{index_.cbegin()};
	auto const index_e{index_.cend()};
	auto other_index_i{other.index_.cbegin()};
	auto const other_index_e{other.index_.cend()};
	auto src_i{other.series(series_idx).cbegin()};

	for (; index_i != index_e && other_index_i != other_index_e;)
	{
		if (*index_i == *other_index_i)
		{
			*dst_i++ = *src_i++;
			++index_i;
			++other_index_i;
		}
		else if (*index_i < *other_index_i)
		{
			++dst_i;
			++index_i;
		}
		else if (*index_i > *other_index_i)
		{
			++other_index_i;
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::_left_join(frame const& other, std::vector<std::size_t> const& series_idxs)
{
	if (series_idxs.empty())
	{
		for (std::size_t idx{0}; idx != other.col_count(); ++idx)
		{
			_left_join(other, idx);
		}
	}
	else
	{
		for (std::size_t idx : series_idxs)
		{
			_left_join(other, idx);
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void shared::data::frame::_outer_join(frame const& other, std::vector<std::size_t> const& series_idxs)
{
	frame result;
	// outer join index
	{
		index_t result_index;
		result_index.reserve(index_.size() + other.index_.size());

		auto index_i{index_.cbegin()};
		auto const index_e{index_.cend()};
		auto other_index_i{other.index_.cbegin()};
		auto const other_index_e{other.index_.cend()};

		for (; index_i != index_e && other_index_i != other_index_e;)
		{
			if (*index_i == *other_index_i)
			{
				result_index.emplace_back(*index_i++);
				++other_index_i;
			}
			else if (*index_i < *other_index_i)
			{
				result_index.emplace_back(*index_i++);
			}
			else if (*index_i > *other_index_i)
			{
				result_index.emplace_back(*other_index_i++);
			}
		}
		for (; index_i != index_e; ++index_i)
		{
			result_index.emplace_back(*index_i);
		}
		for (; other_index_i != other_index_e; ++other_index_i)
		{
			result_index.emplace_back(*other_index_i);
		}
		result_index.shrink_to_fit();
		result.index_.swap(result_index);
	}
	// left join series
	for (std::size_t i{0}; i != col_count(); ++i)
	{
		result._left_join(*this, i);
	}
	result._left_join(other, series_idxs);

	swap(result);
}
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
typename shared::data::frame::series_t& shared::data::frame::series(name_t const& name)
{
	const auto name_i{std::find(series_names_.cbegin(), series_names_.cend(), name)};
	if (name_i == series_names_.cend())
	{
		throw std::runtime_error{"no '"s + name + "' series"s};
	}
	return data_[std::distance(series_names_.cbegin(), name_i)];
}
//-----------------------------------------------------------------------------------------------------
const typename shared::data::frame::series_t& shared::data::frame::series(name_t const& name) const
{
	return const_cast<frame*>(this)->series(name);
}
//-----------------------------------------------------------------------------------------------------
typename shared::data::frame::series_t& shared::data::frame::series(std::size_t idx)
{
	return data_[idx];
}
//-----------------------------------------------------------------------------------------------------
const typename shared::data::frame::series_t& shared::data::frame::series(std::size_t idx) const
{
	return const_cast<frame*>(this)->series(idx);
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::clear() noexcept
{
	index_.clear();
	data_.clear();
	series_names_.clear();
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::reserve(std::size_t size)
{
	index_.reserve(size);
	for (series_t& col : data_)
	{
		col.reserve(size);
	}
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::resize(size_t row_count)
{
	index_.resize(row_count);
	for (series_t& col : data_)
	{
		col.resize(index_.size());
	}
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::swap(frame& other) noexcept
{
	index_.swap(other.index_);
	data_.swap(other.data_);
	series_names_.swap(other.series_names_);
}
//-----------------------------------------------------------------------------------------------------
typename shared::data::frame::series_t* shared::data::frame::create_series(name_t const& name, value_t initial_value)
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
void shared::data::frame::delete_series(std::size_t idx) noexcept
{
	if (idx >= col_count())
	{
		return;
	}
	data_.erase(std::next(data_.begin(), idx));
	series_names_.erase(std::next(series_names_.begin(), idx));
}
//-----------------------------------------------------------------------------------------------------
// returns frame without rows containing nan
shared::data::frame shared::data::frame::clear_lacunas() const
{
	frame result;
	result.data_.resize(data_.size());
	result.series_names_ = series_names_;
	result.reserve(row_count());

	for (std::size_t row{0}; row != row_count(); ++row)
	{
		std::size_t col{0};
		for (; col != col_count() && !std::isnan(data_[col][row]); ++col)
		{}
		if (col == col_count())
		{
			result.index_.emplace_back(index_[row]);
			for (std::size_t col{0}; col != col_count(); ++col)
			{
				result.data_[col].emplace_back(data_[col][row]);
			}
		}
	}
	result._shrink_to_fit();
	return result;
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::print(std::ostream& strm) const
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
	for (std::size_t row{0}; row != row_count(); ++row)
	{
		strm << "\n" << index()[row];
		for (std::size_t col{0}; col != col_count(); ++col)
		{
			strm << ';' << series(col)[row];
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::print_head(std::ostream& strm) const
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
}
//-----------------------------------------------------------------------------------------------------
void shared::data::frame::print_shape(std::ostream& strm) const
{
	strm << "columns: " << col_count() << ", rows: " << row_count() << '\n';
	strm << "index";
	for (std::size_t i{0}; i != col_count(); ++i)
	{
		strm << '\n' << names()[i];
	}
}
