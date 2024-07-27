#pragma once

#include "framework.hpp"

namespace shared::util
{
	//---------------------------------------------------------------------------------------------------------
	// signal_handler
	//---------------------------------------------------------------------------------------------------------
	class signal_handler final
	{
	private:
		inline static std::vector<std::stop_source> stop_sources_;
		inline static std::mutex mtx_;
	private:
		//---------------------------------------------------------------------------------------------------------
		static void _signal_handler(int)
		{
			std::lock_guard<std::mutex> const lock{mtx_};
			std::ranges::for_each(stop_sources_,
				[](std::stop_source& src)
				{
					if (src.stop_possible())
					{
						src.request_stop();
					}
				});
		}
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit signal_handler(std::stop_source stop_source)
		{
			std::lock_guard<std::mutex> const lock{mtx_};
			if (stop_sources_.empty())
			{
				std::signal(SIGINT, _signal_handler);
			}
			stop_sources_.emplace_back(std::move(stop_source));
		}
		//---------------------------------------------------------------------------------------------------------
		~signal_handler()
		{
			std::lock_guard<std::mutex> const lock{mtx_};
			std::signal(SIGINT, SIG_DFL);
			stop_sources_.clear();
		}
	};
}