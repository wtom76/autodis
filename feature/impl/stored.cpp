#include "../pch.hpp"
#include "../shop.hpp"
#include "../index_cursor.hpp"
#include "stored.hpp"
#include "error.hpp"

//---------------------------------------------------------------------------------------------------------
void feature::impl::stored::_init()
{
	shared::data::frame df;
	keeper_dr_->read(std::vector{data_reg_id_}, df);
	if (!df.row_count())
	{
		throw not_enough_data(label());
	}
	auto index_i{std::cbegin(df.index())};
	auto const index_e{std::cend(df.index())};
	auto series_i{std::cbegin(df.series(0))};
	bounds_data bounds;
	for (; index_i != index_e; ++index_i, ++series_i)
	{
		if (!std::isnan(*series_i))
		{
			data_.emplace(*index_i, *series_i);
			if (bounds.value_max_ < *series_i)
			{
				bounds.value_max_ = *series_i;
			}
			if (bounds.value_min_ > *series_i)
			{
				bounds.value_min_ = *series_i;
			}
		}
	}

	bounds.index_min_ = df.index().front();
	bounds.index_max_ = df.index().back();
	_set_bounds(bounds);

	index_cursor c{_mi(), *this, df.index().front()};
	c.next(0);
	bounds_.index_min_ = c.current();
	c.position(df.index().back());
	c.prev(0);
	bounds_.index_max_ = c.current();
}
//---------------------------------------------------------------------------------------------------------
feature::impl::stored::stored(feature_info_t&& info, shop& shop, std::shared_ptr<keeper::data_read> keeper_dr)
	: abstract{std::move(info), shop}
	, data_reg_id_{cfg().at("data_reg_id").get<long long>()}
	, keeper_dr_{std::move(keeper_dr)}
{
	_init();
	assert(!(index_cursor{_mi(), *this, bounds_.index_min_}.on_nan()));
	assert(!(index_cursor{_mi(), *this, bounds_.index_max_}.on_nan()));
}
//---------------------------------------------------------------------------------------------------------
feature::feature_info_t feature::impl::stored::rnd_from_template(feature_info_t const&, shop& shop)
{
	constexpr std::int32_t type_id_stored{1};

	feature::feature_info_t result;
	result.formula_["type"s] = "stored"s;
	result.formula_["data_reg_id"s] = shop.random_feature_info(type_id_stored).id_;
	return result;
}
