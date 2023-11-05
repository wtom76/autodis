#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	// class linear_generator
	//---------------------------------------------------------------------------------------------------------
	class linear_generator
		: public source::feed
	{
	private:
		const std::size_t dst_idx_x_;
		const std::size_t dst_idx_y_;

	private:
		static std::size_t _index(std::span<keeper::feed_args_t const> list, std::string const& value);
		
	public:
		linear_generator(std::span<keeper::feed_args_t const> feed_args);
		//---------------------------------------------------------------------------------------------------------
		// source::feed impl		
		//---------------------------------------------------------------------------------------------------------
		void start(std::unique_ptr<keeper::data_write> dest) override;
		size_t read(std::span<const char> chunk) override;
		void finish(std::span<const char> chunk) override;
		//---------------------------------------------------------------------------------------------------------
		//~source::feed impl		
		//---------------------------------------------------------------------------------------------------------
	};
}