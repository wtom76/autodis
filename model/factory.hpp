#pragma once

#include "framework.hpp"
#include "abstract.hpp"

namespace autodis::model
{
	class factory
	{
	public:
		static void create_model_file(std::string const& model_type, std::filesystem::path const& out_path);
		[[nodiscard]] static std::unique_ptr<abstract> make_unique(std::filesystem::path const& in_path);
	};
}