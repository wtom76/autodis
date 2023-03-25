#pragma once
#include "framework.hpp"

namespace autodis
{
	//----------------------------------------------------------------------------------------------------------
	// class learn_runner
	//----------------------------------------------------------------------------------------------------------
	template <class net>
	class learn_runner
	{
	private:
		double							min_err_{0.01};	// TODO: pass as arg or config
		learning::progress_view			progress_view_;
		typename net::config_t const&	cfg_;
		net&							network_;
		learning::rprop<net>&			teacher_;
		std::jthread					thread_;

	public:
		//----------------------------------------------------------------------------------------------------------
		~learn_runner()
		{
			thread_.join();
		}
		//----------------------------------------------------------------------------------------------------------
		learn_runner(typename net::config_t const& cfg, net& network, learning::rprop<net>& teacher)
			: cfg_{cfg}
			, network_{network}
			, teacher_{teacher}
			, thread_{
				[this](std::stop_token stoken)
				{
					teacher_.teach(cfg_, network_, min_err_, progress_view_, stoken);
					if (stoken.stop_requested())
					{
						return;
					}
					teacher_.show_test(network_, progress_view_, stoken);
				}
			}
		{
		}
	};
}
