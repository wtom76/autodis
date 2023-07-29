#pragma once
#include "framework.hpp"

namespace autodis
{
	//----------------------------------------------------------------------------------------------------------
	// class learn_runner
	//----------------------------------------------------------------------------------------------------------
	template <class net>
	class learn_runner
		: public learning::progress_view
	{
	private:
		double							min_err_{0.001};	// TODO: pass as arg or config
		typename net::config_t const&	cfg_;
		net&							network_;
		learning::rprop<net>&			teacher_;
		std::jthread					thread_;

	public:
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
		learn_runner(typename net::config_t const& cfg, net& network, learning::rprop<net>& teacher)
			: cfg_{cfg}
			, network_{network}
			, teacher_{teacher}
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
		}
	};
}
