#pragma once

#include "framework.hpp"
#include <shared/math/normalization.hpp>
#include "error.hpp"

namespace feature
{
	class shop;

	//---------------------------------------------------------------------------------------------------------
	// abstract
	//---------------------------------------------------------------------------------------------------------
	class abstract
		: protected shared::util::logged
	{
	// types
	public:
		using type_id_t			= std::int32_t;
		using normalization_t	= shared::math::tanh_normalization;

		struct bounds_data
		{
			index_value_t index_min_{1};	// 1 is to make default range empty
			index_value_t index_max_{0};	// inclusive
			value_t value_min_{std::numeric_limits<value_t>::max()};
			value_t value_max_{std::numeric_limits<value_t>::lowest()};

			bounds_data() noexcept = default;
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
		shop&					shop_;
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
		static void _verify_typeset(std::vector<std::int32_t> const& type_ids, std::string const& label);
		//---------------------------------------------------------------------------------------------------------
		explicit abstract(feature_info_t&& info, shop& shop)
			: info_{std::move(info)}
			, shop_{shop}
		{}
	public:
		//---------------------------------------------------------------------------------------------------------
		virtual ~abstract(){}
		//---------------------------------------------------------------------------------------------------------
		void dump(std::filesystem::path const& out_path) const;
		//---------------------------------------------------------------------------------------------------------
		feature_info_t const& info() const noexcept { return info_; }
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