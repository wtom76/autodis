#pragma once

#include "framework.hpp"
#include <keeper/keeper.hpp>
#include <shared/math/normalization.hpp>
#include "error.hpp"

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// abstract
	//---------------------------------------------------------------------------------------------------------
	class abstract
	{
	// types
	public:
		using feature_info_t	= keeper::metadata::feature_info;
		using normalization_t	= shared::math::tanh_normalization;

		struct bounds_data
		{
			index_value_t index_min_{1};	// 1 is to make default range empty
			index_value_t index_max_{0};	// inclusive
			value_t value_min_{};
			value_t value_max_{};

			bounds_data() noexcept = default;
			bounds_data(keeper::data_read::bounds const& src) noexcept
				: index_min_{src.index_min_}
				, index_max_{src.index_max_}
				, value_min_{src.value_min_}
				, value_max_{src.value_max_}
			{}
			bool test(index_value_t idx_val) const noexcept
			{
				return idx_val >= index_min_ && idx_val <= index_max_;
			}
		};
	private:
		using data_t = std::unordered_map<index_value_t, value_t>;

	// data
	protected:
		feature_info_t const	info_;
		bounds_data				bounds_;
		normalization_t			norm_;
		data_t					data_;

	// methods
	protected:
		//---------------------------------------------------------------------------------------------------------
		void _set_bounds(bounds_data b)
		{
			bounds_ = b;
			norm_ = shared::math::tanh_normalization{shared::math::min_max{bounds_.value_min_, bounds_.value_max_}};
		}
		//---------------------------------------------------------------------------------------------------------
		explicit abstract(feature_info_t&& info)
			: info_{std::move(info)}
		{}
	public:
		//---------------------------------------------------------------------------------------------------------
		virtual ~abstract(){}
		//---------------------------------------------------------------------------------------------------------
		nlohmann::json const& cfg() const noexcept { return info_.formula_; }
		//---------------------------------------------------------------------------------------------------------
		[[nodiscard]] std::string label() const noexcept { return info_.label_; }
		//---------------------------------------------------------------------------------------------------------
		bounds_data const& bounds() const noexcept { return bounds_; }
		//---------------------------------------------------------------------------------------------------------
		normalization_t const& norm() const noexcept { return norm_; }
		//---------------------------------------------------------------------------------------------------------
		// 1. return value if exists
		// 2. or return NaN.
		[[nodiscard]] value_t value(index_value_t idx_val) const
		{
			if (!bounds_.test(idx_val))
			{
				throw feature_out_of_bounds{idx_val};
			}
			auto const data_i{data_.find(idx_val)};
			return data_i != data_.cend()
				? data_i->second
				: shared::data::nan;
		}
	};
}