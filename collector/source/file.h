#pragma once

#include "base.h"

namespace collector::source
{
	//---------------------------------------------------------------------------------------------------------
	/// class file
	//---------------------------------------------------------------------------------------------------------
	class file
		: public base
	{
		static constexpr size_t buf_size_{4 * 1024 * 1024};
		std::ifstream f_;
	public:
		file(const std::filesystem::path& path);

		void fetch_to(feed& dest) override;
	};
}