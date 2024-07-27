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
		inline static std::mutex mtx_;
		inline static std::list<std::stop_source> stop_sources_;

		std::stop_source src_;
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
			stop_sources_.clear();
		}
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit signal_handler(std::stop_source stop_source)
			: src_{std::move(stop_source)}
		{
			std::lock_guard<std::mutex> const lock{mtx_};
			if (stop_sources_.empty())
			{
				std::signal(SIGINT, _signal_handler);
			}
			stop_sources_.emplace_back(src_);
		}
		//---------------------------------------------------------------------------------------------------------
		~signal_handler()
		{
			std::lock_guard<std::mutex> const lock{mtx_};
			if (auto const iter{std::ranges::find(stop_sources_, src_)}; iter != stop_sources_.cend())
			{
				stop_sources_.erase(iter);
				if (stop_sources_.empty())
				{
					std::signal(SIGINT, SIG_DFL);
				}
			}
		}
	};
}