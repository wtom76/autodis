#pragma once

#include "framework.hpp"
#include "base.hpp"

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class rest
	//---------------------------------------------------------------------------------------------------------
	class rest
		: public base
	{
	private:
		std::string const	url_;
		feed*				dest_{nullptr};

	private:
		void _data_callback(char const* data, size_t size, size_t nmemb);
	public:
		rest(std::string url);

		// base impl
		void fetch_to(feed& dest) override;
		//~base impl

		// curl callbacks
		static size_t data_callback(char* data, size_t size, size_t nmemb, void* userdata);
		//~curl callbacks
	};
}