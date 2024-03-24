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
		static void _center(std::vector<double>& scatter) noexcept
		{
			auto const mm{std::ranges::minmax(scatter)};
			double const shift{-(mm.max + mm.min) / 2};
			std::ranges::transform(scatter, std::begin(scatter), [shift](double val){ return val + shift; });
		}
		//---------------------------------------------------------------------------------------------------------
		void _run_row(std::size_t row, col_scatter_t& result)
		{
			result.clear();
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
		void run(prediction_context_t& ctx, std::size_t max_rows_to_use)
		{
			ctx_ = &ctx;
			result_.clear();
			result_.resize(ctx_->input_filler().col_count());
			double const rows_available{static_cast<double>(ctx_->input_filler().row_count())};
			double const row_step{max_rows_to_use < rows_available ? static_cast<double>(rows_available) / max_rows_to_use : 1.};
			col_scatter_t col_scatter;
			for (double row{0.}; row < rows_available; row += row_step)
			{
				std::size_t row_idx{static_cast<std::size_t>(row)};
				assert(row_idx < ctx_->input_filler().row_count());

				_run_row(row_idx, col_scatter);
				
				for (std::size_t col{0}; col != ctx_->input_filler().col_count(); ++col)
				{
					_center(col_scatter[col]);
					for (double point : col_scatter[col])
					{
						result_[col].emplace_back(point);
					}
				}
				SPDLOG_LOGGER_INFO(log(), "building partial_dependence... row {}", row_idx);
			}
		}
		//---------------------------------------------------------------------------------------------------------
		col_scatter_t const& result() const noexcept { return result_; }
		//---------------------------------------------------------------------------------------------------------
		prediction_context_t const& context() const noexcept { assert(ctx_); return *ctx_; }
		//---------------------------------------------------------------------------------------------------------
		void write_sorted_result(std::ostream& dst, std::vector<std::string> const& names)
		{
			std::vector<double> ranges(result_.size());
			std::ranges::transform(result_, std::begin(ranges),
				[](std::vector<double> const& scatter) constexpr
				{
					auto const minmax{std::ranges::minmax(scatter)};
					return minmax.max - minmax.min;
				});
			std::vector<std::size_t> indexes(ranges.size());
			std::iota(std::begin(indexes), std::end(indexes), 0);
			std::ranges::sort(indexes,
				[&ranges](std::size_t left, std::size_t right) constexpr
				{
					return ranges[left] < ranges[right];
				});

			dst << "series"sv << ';' << "range"sv << '\n';
			for (std::size_t col : indexes)
			{
				auto const mm_pr{std::ranges::minmax(result_[col])};
				dst << names[col] << ';' << (mm_pr.max - mm_pr.min) << '\n';
			}
		}
	};
}