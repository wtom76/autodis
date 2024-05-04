#include "pch.hpp"
#include "util.hpp"
#include "error.hpp"

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	std::pair<master_index::index_value_t, std::ptrdiff_t> _solid_next_neg(
		master_index const& mi,
		abstract& feat,
		master_index::index_value_t start,
		std::ptrdiff_t max_distance)
	{
		assert(max_distance < 0);
		master_index::index_pos_t mi_pos{mi.pos(start)};
		std::ptrdiff_t distance_left{max_distance};
		master_index::index_pos_t const mi_term_pos{mi.pos(feat.bounds().index_min_)};

		for (; mi_term_pos < mi_pos && distance_left; --mi_pos)
		{
			if (!std::isnan(feat.value(mi.at(mi_pos))))
			{
				++distance_left;														// max_distance is negative
			}
		}
		return {mi.at(mi_pos), max_distance - distance_left};
	}
	//---------------------------------------------------------------------------------------------------------
	std::pair<master_index::index_value_t, std::ptrdiff_t> _solid_next_pos(
		master_index const& mi,
		abstract& feat,
		master_index::index_value_t start,
		std::ptrdiff_t max_distance)
	{
		assert(max_distance > 0);
		master_index::index_pos_t mi_pos{mi.pos(start)};
		std::ptrdiff_t distance_left{max_distance};
		master_index::index_pos_t const mi_term_pos{mi.pos(feat.bounds().index_max_)};

		for (; mi_term_pos > mi_pos && distance_left; ++mi_pos)
		{
			if (!std::isnan(feat.value(mi.at(mi_pos))))
			{
				--distance_left;
			}
		}
		return {mi.at(mi_pos), max_distance - distance_left};
	}
	//---------------------------------------------------------------------------------------------------------
	std::pair<master_index::index_value_t, std::ptrdiff_t> solid_next(
		master_index const& mi,
		abstract& feat,
		master_index::index_value_t start,
		std::ptrdiff_t max_distance)
	{
		if (!max_distance)
		{
			return {start, 0};
		}
		if (max_distance > 0)
		{
			return _solid_next_pos(mi, feat, start, max_distance);
		}
		else
		{
			return _solid_next_neg(mi, feat, start, max_distance);
		}
	}
}
