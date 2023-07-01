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
		struct candles
		{
			std::array<size_t, 4>	ohlc_;
			scale_y					scale_y_;
		};

	private:
		std::future<void>		fut_;
		sf::Window				window_;
		data_frame_t const&		df_;
		scale_date				scale_x_;
		std::vector<candles>	candlesticks_;
	
	private:
		void _wait();
		shared::math::range _min_max(std::array<size_t, 4> const& ohlc_idc) const noexcept;

	public:
		explicit chart(shared::data::frame const& df);
		~chart();

		void set_candlesticks(std::array<size_t, 4> const& ohlc_idc);

		void show();
	};
}