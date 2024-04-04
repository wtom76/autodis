#pragma once

#include "framework.hpp"
#include <keeper/keeper.hpp>
#include <shared/math/normalization.hpp>

namespace feature
{
	//---------------------------------------------------------------------------------------------------------
	// abstract
	//---------------------------------------------------------------------------------------------------------
	class abstract
	{
	// types
	public:
		using index_value_t = shared::data::frame::index_value_t;
		using value_t = shared::data::frame::value_t;

		struct bounds
		{
			index_value_t index_min_{1};	// 1 is to make default range empty
			index_value_t index_max_{0};	// inclusive
			value_t value_min_{};
			value_t value_max_{};

			bounds() noexcept = default;
			bounds(keeper::data_read::bounds const& src) noexcept
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
	private:
		std::shared_mutex mtx_;
	protected:
		nlohmann::json const cfg_;
		bounds bounds_;
		shared::math::tanh_normalization norm_;
		data_t data_;

	// methods
	private:
		//---------------------------------------------------------------------------------------------------------
		// should insert into data_ at least {idx_val, value} pair and possibly more, may be even all available values
		// should return value bound to idx_val
		virtual value_t _evaluate(index_value_t idx_val) = 0;
		//---------------------------------------------------------------------------------------------------------
		// since transfer from shared to exclusive lock is not atomic, should check again under unique_lock
		// if we are actually first to call _evaluate for this idx_val
		[[gnu::cold, nodiscard]]
		value_t _call_evaluate(index_value_t idx_val)
		{
			std::unique_lock lock{mtx_};
			auto const data_i{data_.find(idx_val)};
			if (data_i != data_.cend())
			{
				return data_i->second;
			}
			return _evaluate(idx_val);
		}
	protected:
		//---------------------------------------------------------------------------------------------------------
		void _set_bounds(bounds b)
		{
			bounds_ = b;
			norm_ = shared::math::tanh_normalization{shared::math::min_max{bounds_.value_min_, bounds_.value_max_}};
		}
		//---------------------------------------------------------------------------------------------------------
		explicit abstract(nlohmann::json cfg)
			: cfg_{std::move(cfg)}
		{}
	public:
		//---------------------------------------------------------------------------------------------------------
		virtual ~abstract(){}
		//---------------------------------------------------------------------------------------------------------
		nlohmann::json const& cfg() const noexcept { return cfg_; }
		//---------------------------------------------------------------------------------------------------------
		bounds const& bounds() const noexcept { return bounds_; };
		//---------------------------------------------------------------------------------------------------------
		// 1. return value if exists
		// 2. or ask impl to evaluate requested value and return it.
		[[nodiscard]] value_t value(index_value_t idx_val)
		{
			assert(bounds_.test(idx_val));
			// 1.
			{
				std::shared_lock lock{mtx_};
				auto const data_i{data_.find(idx_val)};
				if (data_i != data_.cend())
				{
					return data_i->second;
				}
			}
			// 2.
			return _call_evaluate(idx_val);
		}
		//---------------------------------------------------------------------------------------------------------
		// return normalized value()
		[[nodiscard]] value_t normalized_value(index_value_t idx_val)
		{
			return norm_.normalize(value(idx_val));
		}
		//---------------------------------------------------------------------------------------------------------
		// return denormalized value
		[[nodiscard]] value_t restore_value(value_t val)
		{
			return norm_.restore(val);
		}
	};
}