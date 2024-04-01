#pragma once

#include "framework.hpp"

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

		struct index_bounds
		{
			index_value_t first_{1};	// 1 is to make default range lengthless
			index_value_t last_{0};		// inclusive

			index_bounds(std::pair<index_value_t, index_value_t> pr)
				: first_{pr.first}
				, last_{pr.second}
			{}
			bool test(index_value_t idx_val) const noexcept
			{
				return idx_val >= first_ && idx_val <= last_;
			}
		};
	private:
		using data_t = std::unordered_map<index_value_t, value_t>;

	// data
	private:
		std::shared_mutex mtx_;
	protected:
		nlohmann::json const cfg_;
		index_bounds const bounds_;
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
		explicit abstract(nlohmann::json cfg, index_bounds bounds)
			: cfg_{std::move(cfg)}
			, bounds_{bounds}
		{}
	public:
		//---------------------------------------------------------------------------------------------------------
		virtual ~abstract(){}
		//---------------------------------------------------------------------------------------------------------
		nlohmann::json const& cfg() const noexcept { return cfg_; }
		//---------------------------------------------------------------------------------------------------------
		index_bounds const& bounds() const noexcept { return bounds_; };
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
	};
}