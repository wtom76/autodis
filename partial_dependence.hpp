#pragma once

#include "framework.hpp"
#include <learning/sample_filler.hpp>

namespace autodis
{
	//---------------------------------------------------------------------------------------------------------
	template <class ct>
	concept prediction_context_c = requires (ct a)
	{
		{ a.input_filler() } -> std::same_as<learning::sample_filler&>;
		a.network();
		a.data_view();
	};
	//---------------------------------------------------------------------------------------------------------
	// partial_dependence
	//---------------------------------------------------------------------------------------------------------
	template <prediction_context_c prediction_context>
	class partial_dependence
		: private shared::util::logged
	{
	public:
		using prediction_context_t	= prediction_context;
		using col_scatter_t			= std::vector<std::vector<double>>;
		
	private:
		prediction_context_t*	ctx_{nullptr};
		std::vector<double>		input_;
		col_scatter_t			result_;


	private:
		//---------------------------------------------------------------------------------------------------------
		void _run_row(std::size_t row, std::vector<std::vector<double>>& result)
		{
			result.resize(ctx_->input_filler().col_count());
			ctx_->input_filler().fill_to(row, input_);
			for (std::size_t col{0}; col != ctx_->input_filler().col_count(); ++col)
			{
				_run_cell(col, result[col]);
			}
		}
		//---------------------------------------------------------------------------------------------------------
		void _run_cell(std::size_t col, std::vector<double>& result)
		{
			constexpr double input_min{-1.};
			constexpr double input_max{1.};
			constexpr double input_step{0.1};

			result.reserve(static_cast<std::size_t>((input_max - input_min) / input_step));
			double const original_cell_value{input_[col]};
			for (double value{input_min}; value <= input_max; value += input_step)
			{
				input_[col] = value;
				ctx_->input_filler().fill_from(input_);
				ctx_->network().forward();
				result.emplace_back(ctx_->network().omega_layer().front());
			}
			input_[col] = original_cell_value;
		}
	public:
		//---------------------------------------------------------------------------------------------------------
		void run(prediction_context_t& ctx, size_t row_step)
		{
			ctx_ = &ctx;
			result_.clear();
			result_.resize(ctx_->input_filler().col_count());
			col_scatter_t cell_scatter;
			for (std::size_t row{0}; row < ctx_->input_filler().row_count(); row += row_step)
			{
				_run_row(row, cell_scatter);
				for (std::size_t col{0}; col != ctx_->input_filler().col_count(); ++col)
				{
					for (double point : cell_scatter[col])
					{
						result_[col].emplace_back(point);
					}
				}
				SPDLOG_LOGGER_INFO(log(), "building partial_dependence... row {}", row);
			}
		}
		//---------------------------------------------------------------------------------------------------------
		col_scatter_t const& result() const noexcept { return result_; }
		//---------------------------------------------------------------------------------------------------------
		prediction_context_t const& context() const noexcept { assert(ctx_); return *ctx_; }
	};
}