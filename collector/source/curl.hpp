#pragma once

#include <curl/curl.h>

namespace collector::source
{
	class curl_easy
	{
		CURL* eh_{nullptr};
	public:
		curl_easy();
		curl_easy(curl_easy&& src);

		curl_easy& operator = (curl_easy&& src) = delete;
		curl_easy(const curl_easy&) = delete;
		curl_easy& operator = (const curl_easy&) = delete;

		~curl_easy();

		CURL* handle() const noexcept { return eh_; }
	};
}