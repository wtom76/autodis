#pragma once

namespace shared::util
{
	//---------------------------------------------------------------------------------------------------------
	/// class uri
	//---------------------------------------------------------------------------------------------------------
	class uri
	{
		std::vector<std::string> parts_;
		
	public:
		uri() = default;
		uri(std::string src);

		std::vector<std::string> const& parts() const noexcept { return parts_; }
	};
}