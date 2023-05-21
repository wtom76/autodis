#pragma once
#include "framework.hpp"
#include "source/feed.hpp"
#include "source/base.hpp"
#include "feed_uri.hpp"
#include "source_uri.hpp"
#include <keeper/keeper.hpp>

namespace collector
{
	//----------------------------------------------------------------------------------------------------------
	/// class factory
	/// creates source and feed by feed uri
	//----------------------------------------------------------------------------------------------------------
	class factory final
	{
	private:
		static std::unique_ptr<source::base> _create_file(std::string file_name);
	public:
		static std::unique_ptr<source::feed> feed(keeper::metadata::feed_info const& info);
		static std::unique_ptr<source::base> source(source_uri const& uri, std::string const& source_args);
	};
}