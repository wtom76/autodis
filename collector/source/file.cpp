#include "pch.hpp"

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include "file.hpp"

//---------------------------------------------------------------------------------------------------------
collector::source::file::file(const std::filesystem::path& path)
	: f_{path}
{
	if (!f_)
	{
		throw std::runtime_error{"can't open "s + path.native() + ". "s + std::strerror(errno)};
	}
}
//---------------------------------------------------------------------------------------------------------
void collector::source::file::fetch_to(feed& dest)
{
	dest.start();
	std::vector<char> buffer(buf_size_);
	std::ptrdiff_t size_buffered{0};
	while (f_.good())
	{
		const std::ptrdiff_t size_available{static_cast<std::ptrdiff_t>(buffer.size()) - size_buffered};
		if (size_available <= 0)
		{
			throw std::runtime_error{"can't parse buffer of size "s + std::to_string(buf_size_)};
		}
		f_.read(buffer.data() + size_buffered, size_available);
		const std::ptrdiff_t size_read{f_.gcount()};
		if (size_read <= 0)
		{
			throw std::runtime_error{"parse stale"s};
		}
		size_buffered += size_read;
		const size_t size_parsed{dest.read({buffer.data(), static_cast<size_t>(size_buffered)})};
		if (size_parsed > 0)
		{
			size_buffered -= size_parsed;
			std::memmove(buffer.data(), buffer.data() + size_parsed, size_buffered);
		}
	}
	dest.finish({buffer.data(), static_cast<size_t>(size_buffered)});
}
