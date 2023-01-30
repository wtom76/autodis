#include "pch.h"
#include "uri.h"

//---------------------------------------------------------------------------------------------------------
shared::util::uri::uri(std::string src)
{
	std::stringstream s{std::move(src)};
	std::string part;
	while (std::getline(s, part, '/'))
	{
		parts_.emplace_back(std::move(part));
	}
}
