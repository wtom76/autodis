#pragma once
#include "source/feed.h"
#include "source/base.h"
#include "feed_uri.hpp"
#include "source_uri.hpp"

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
		static std::unique_ptr<source::feed> feed(feed_uri const& uri);
		static std::unique_ptr<source::base> source(source_uri const& uri);
	};
}