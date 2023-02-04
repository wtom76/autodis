#include "pch.h"
#include "uri.h"

//---------------------------------------------------------------------------------------------------------
void shared::util::uri::_parse(std::string src, std::vector<std::string>& dest)
{
	std::stringstream s{std::move(src)};
	std::string part;
	while (std::getline(s, part, '/'))
	{
		dest.emplace_back(std::move(part));
	}
}
//---------------------------------------------------------------------------------------------------------
std::vector<std::string> shared::util::uri::_parse(std::string src)
{
	std::vector<std::string> result;
	_parse(std::move(src), result);
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::vector<std::string> shared::util::uri::_parse(std::initializer_list<std::string> src)
{
	std::vector<std::string> result;
	for (const std::string& str : src)
	{
		_parse(std::move(str), result);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
shared::util::uri::uri(std::string src)
	: parts_{_parse(std::move(src))}
{}
//---------------------------------------------------------------------------------------------------------
shared::util::uri::uri(std::initializer_list<std::string> src)
	: parts_{_parse(std::move(src))}
{}
//---------------------------------------------------------------------------------------------------------
std::string const& shared::util::uri::part(std::size_t idx) const
{
	if (idx >= parts_.size())
	{
		throw std::runtime_error("part index ("s + std::to_string(idx) + ") is out of uri range "s + to_string());
	}
	return parts_[idx];
}
//---------------------------------------------------------------------------------------------------------
std::string shared::util::uri::to_string() const
{
	auto part_i{parts_.cbegin()};
	if (part_i == parts_.cend())
	{
		return {};
	}
	std::string result{*part_i++};
	for (; part_i != parts_.cend(); ++part_i)
	{
		result += '/';
		result += *part_i;
	}
	return result;
}
