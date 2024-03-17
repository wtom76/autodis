#pragma once
#include "framework.hpp"

namespace autodis
{
	//----------------------------------------------------------------------------------------------------------
	// class learn_runner
	//----------------------------------------------------------------------------------------------------------
	template <class net, class target_normalization = shared::math::tanh_normalization>
	class learn_runner
		: public learning::progress_view
	{
	public:
		using series_view_t = shared::data::view::series_view_t;
		using target_normalization_t = target_normalization;
		using chart_t = autodis::visual::chart;
		using store_result_network_t = std::function<void(nlohmann::json&&)>;
	
	private:
		double							min_err_{0.001};	// TODO: pass as arg or config
		typename net::config_t const&	cfg_;
		net&							network_;
		learning::rprop<net>&			teacher_;

		learning::sample_filler const*	inputs_filler_;
		series_view_t*					prediction_;
		target_normalization_t const	norm_;
		chart_t*						chrt_{nullptr};

		store_result_network_t			store_result_network_;

		std::jthread					thread_;			// keep it last member

	private:
		//----------------------------------------------------------------------------------------------------------
		void _update_chart()
		{
			if (!chrt_)
			{
				return;
			}
			assert(prediction_);
			assert(inputs_filler_);
			size_t row{0};
			for (double& dest : *prediction_)
			{
				inputs_filler_->fill(row++);
				network_.forward();
				dest = network_.omega_layer().front();
			}
			chrt_->invalidate();
		}

	public:
		//----------------------------------------------------------------------------------------------------------
		learn_runner(
			typename net::config_t const& cfg,
			net& network,
			learning::rprop<net>& teacher,
			learning::sample_filler const& inputs_filler,
			series_view_t& prediction,
			target_normalization_t const& norm,
			chart_t& chrt,
			store_result_network_t&& store_result_network)
			: cfg_{cfg}
			, network_{network}
			, teacher_{teacher}
			, inputs_filler_{&inputs_filler}
			, prediction_{&prediction}
			, norm_{norm}
			, chrt_{&chrt}
			, store_result_network_{std::move(store_result_network)}
			, thread_{
				[this](std::stop_token stoken)
				{
					teacher_.teach(cfg_, network_, min_err_, *this, stoken);
					if (stoken.stop_requested())
					{
						return;
					}
					teacher_.show_test(network_, *this, stoken);
				}
			}
		{
		}
		//----------------------------------------------------------------------------------------------------------
		~learn_runner()
		{
			try
			{
				wait();
			}
			catch (std::exception const& ex)
			{
				SPDLOG_LOGGER_ERROR(log(), ex.what());
			}
		}
		//----------------------------------------------------------------------------------------------------------
		void wait()
		{
			if (thread_.joinable())
			{
				thread_.join();
			}
		}
		//----------------------------------------------------------------------------------------------------------
		// learning::progress_view overrides
		void set_best(double min_err) override
		{
			learning::progress_view::set_best(min_err);
			nlohmann::json j = network_;
			store_result_network_(std::move(network_));
			//std::ofstream{"best_net.dump"s} << network_;
			_update_chart();
		}
	};
}
