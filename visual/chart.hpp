#pragma once

#include "framework.hpp"
#include "scale_date.hpp"
#include "scale_y.hpp"

namespace autodis::visual
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class chart
		: private shared::util::logged
	{
	private:
		static constexpr size_t max_scales_y_{1024};

		template <typename type>
		struct point_2d
		{
			type x_{0};				
			type y_{0};				
		};
		using color_t = std::array<float, 3>;
		using data_frame_t = shared::data::frame;
		struct line
		{
			size_t	series_idx_{0};
			size_t	scale_y_idx_{0};
			color_t	color_;
		};
		struct candles
		{
			std::array<size_t, 4>	ohlc_;		// series indices
			size_t					scale_y_idx_{0};
		};
		struct mouse
		{
			bool			active_{false};
			point_2d<int>	start_point_;
			point_2d<int>	current_point_;
		};

		class gl_context;

	private:
		std::future<void>		fut_;
		point_2d<uint32_t>		window_size_{800, 600};
		mouse					mouse_;
		sf::Window				window_;
		data_frame_t const&		df_;
		scale_date				scale_x_;
		std::vector<scale_y>	scales_y_;
		std::vector<line>		lines_;
		std::vector<candles>	candlesticks_;

		std::unique_ptr<gl_context> gl_ctx_;
	
	private:
		void _wait();
		shared::math::range _min_max(std::array<size_t, 4> const& ohlc_idc) const noexcept;

		std::unique_ptr<gl_context> _create_ctx() const;
		void _fill_ctx(gl_context& ctx) const;
		void _add_scale_y(size_t scale_y_idx, shared::math::range rng);
		size_t _add_line(line const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const;
		size_t _add_candles(candles const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const;

	public:
		explicit chart(shared::data::frame const& df);
		~chart();

		// valid scale_y_idx is in range [0..max_scales_y_]
		void add_line(size_t scale_y_idx, size_t series_idx, color_t color);
		// valid scale_y_idx is in range [0..max_scales_y_]
		void add_candlesticks(size_t scale_y_idx, std::array<size_t, 4> const& ohlc_idc);

		void show();
	};
}