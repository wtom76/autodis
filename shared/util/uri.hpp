#pragma once

#include "framework.hpp"

namespace shared::util
{
	//---------------------------------------------------------------------------------------------------------
	/// class uri
	//---------------------------------------------------------------------------------------------------------
	class uri
	{
		std::vector<std::string> parts_;

	private:
		static void _parse(std::string src, std::vector<std::string>& dest);
		static std::vector<std::string> _parse(std::string src);
		static std::vector<std::string> _parse(std::initializer_list<std::string> src);
		
	public:
		uri(std::string src);
		uri(std::initializer_list<std::string> src);

		std::vector<std::string> const& parts() const noexcept { return parts_; }
		std::string const& part(std::size_t idx) const;
		std::string to_string() const;
	};
}