#include "pch.hpp"
#include "curl.hpp"

//---------------------------------------------------------------------------------------------------------
collector::source::curl_easy::curl_easy()
	: eh_{curl_easy_init()}
{
	if (!eh_)
	{
		throw std::runtime_error("curl_easy_init failed"s);
	}
}
//---------------------------------------------------------------------------------------------------------
collector::source::curl_easy::curl_easy(curl_easy&& src)
	: eh_{src.eh_}
{
	src.eh_ = nullptr;
}
//---------------------------------------------------------------------------------------------------------
collector::source::curl_easy::~curl_easy()
{
	if (eh_)
	{
		curl_easy_cleanup(eh_);
		eh_ = nullptr;
	}
}
