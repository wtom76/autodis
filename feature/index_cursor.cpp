#include "pch.hpp"
#include "index_cursor.hpp"
#include "error.hpp"

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// index_cursor
	//---------------------------------------------------------------------------------------------------------
	index_cursor::index_cursor(master_index const& mi, abstract& feature, index_value_t start)
		: mi_{mi}
		, feature_{feature}
		, pos_{mi_.pos(start)}
		, min_pos_{mi_.pos(feature_.bounds().index_min_)}
		, max_pos_{mi_.pos(feature_.bounds().index_max_)}
	{}
	//---------------------------------------------------------------------------------------------------------
	std::ptrdiff_t index_cursor::_next(std::ptrdiff_t max_distance)
	{
		assert(max_distance >= 0);
		std::ptrdiff_t distance_left{max_distance};
		while (max_pos_ > pos_ && on_nan())
		{
			++pos_;
		}
		while (max_pos_ > pos_ && distance_left)
		{
			++pos_;
			if (!on_nan())
			{
				--distance_left;
			}
		}
		return max_distance - distance_left;
	}
	//---------------------------------------------------------------------------------------------------------
	std::ptrdiff_t index_cursor::_prev(std::ptrdiff_t max_distance)
	{
		assert(max_distance >= 0);
		std::ptrdiff_t distance_left{max_distance};
		while (min_pos_ < pos_ && on_nan())
		{
			--pos_;
		}
		while (min_pos_ < pos_ && distance_left)
		{
			--pos_;
			if (!on_nan())
			{
				--distance_left;
			}
		}
		return max_distance - distance_left;
	}
	//---------------------------------------------------------------------------------------------------------
	bool index_cursor::on_nan() const
	{
		return std::isnan(feature_.value(mi_.at(pos_)));
	}
	//---------------------------------------------------------------------------------------------------------
	void index_cursor::position(index_value_t index_value)
	{
		pos_ = mi_.pos(index_value);
	}
	//---------------------------------------------------------------------------------------------------------
	std::ptrdiff_t index_cursor::next(std::ptrdiff_t max_distance)
	{
		return max_distance >= 0 ? _next(max_distance) : -_prev(-max_distance);
	}
	//---------------------------------------------------------------------------------------------------------
	std::ptrdiff_t index_cursor::next()
	{
		return next(1);
	}
	//---------------------------------------------------------------------------------------------------------
	std::ptrdiff_t index_cursor::prev(std::ptrdiff_t max_distance)
	{
		return max_distance >= 0 ? _prev(max_distance) : -_next(-max_distance);
	}
	//---------------------------------------------------------------------------------------------------------
	std::ptrdiff_t index_cursor::prev()
	{
		return prev(1);
	}
	//---------------------------------------------------------------------------------------------------------
	index_value_t index_cursor::current() const
	{
		return mi_.at(pos_);
	}
}
