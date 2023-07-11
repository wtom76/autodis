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
		using data_frame_t = shared::data::frame;
		struct line
		{
			size_t	series_idx_;
			scale_y	scale_y_;
		};
		struct candles
		{
			std::array<size_t, 4>	ohlc_;		// series indices
			scale_y					scale_y_;
		};

		class gl_context;

	private:
		std::future<void>		fut_;
		sf::Window				window_;
		data_frame_t const&		df_;
		scale_date				scale_x_;
		std::vector<line>		lines_;
		std::vector<candles>	candlesticks_;

		std::unique_ptr<gl_context> gl_ctx_;
	
	private:
		void _wait();
		shared::math::range _min_max(std::array<size_t, 4> const& ohlc_idc) const noexcept;

		std::unique_ptr<gl_context> _create_ctx() const;
		size_t _add_line(line const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const;
		size_t _add_candles(candles const& src, std::vector<float>& dst_verices, std::vector<float>& dst_colors) const;

	public:
		explicit chart(shared::data::frame const& df);
		~chart();

		void add_line(size_t series_idx);
		void add_candlesticks(std::array<size_t, 4> const& ohlc_idc);

		void show();
	};
}