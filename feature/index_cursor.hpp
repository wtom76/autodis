#pragma once

#include "framework.hpp"
#include "master_index.hpp"
#include "abstract.hpp"

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// class index_cursor
	// iterates only those master index positions at which feature has valis values
	//---------------------------------------------------------------------------------------------------------
	class index_cursor
	{
	private:
		master_index const& mi_;
		abstract&			feature_;
		index_pos_t			pos_{0};
		index_pos_t			min_pos_{0};
		index_pos_t			max_pos_{0};

	private:
		std::ptrdiff_t _next(std::ptrdiff_t max_distance);
		std::ptrdiff_t _prev(std::ptrdiff_t max_distance);

	public:
		explicit index_cursor(master_index const& mi, abstract& feature, index_value_t start);
		// is feature value at current position is NaN
		bool on_nan() const;
		// set cursor in position corresponding to provided index value
		void position(index_value_t index_value);
		std::ptrdiff_t next(std::ptrdiff_t max_distance);
		std::ptrdiff_t next();
		std::ptrdiff_t prev(std::ptrdiff_t max_distance);
		std::ptrdiff_t prev();
		[[nodiscard]] index_value_t current() const;
	};
}