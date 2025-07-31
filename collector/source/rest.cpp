#include "pch.hpp"
#include "rest.hpp"
#include "curl.hpp"

//---------------------------------------------------------------------------------------------------------
collector::source::rest::rest(std::string source_args)
	: url_{std::move(source_args)}
{}
//---------------------------------------------------------------------------------------------------------
void collector::source::rest::fetch_to(feed& dest)
{
	dest_ = &dest;
	dest_->start();
	{
		curl_easy ce;
		curl_easy_setopt(ce.handle(), CURLOPT_WRITEDATA, static_cast<void*>(this));
		curl_easy_setopt(ce.handle(), CURLOPT_WRITEFUNCTION, data_callback);
		if (curl_easy_setopt(ce.handle(), CURLOPT_URL, url_.c_str()) != CURLE_OK)
		{
			throw std::runtime_error("rest source failed on setting url"s);
		}
		curl_easy_perform(ce.handle());
	}
	dest_->finish({});
	dest_ = nullptr;
}
//---------------------------------------------------------------------------------------------------------
void collector::source::rest::_data_callback(char const* data, size_t size, size_t nmemb)
{
	assert(dest_);
	dest_->read(std::span{data, size * nmemb});
}
//---------------------------------------------------------------------------------------------------------
std::size_t collector::source::rest::data_callback(char* data, size_t size, size_t nmemb, void* userdata)
{
	static_cast<rest*>(userdata)->_data_callback(data, size, nmemb);
	return size * nmemb;
}
