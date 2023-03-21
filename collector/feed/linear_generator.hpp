#pragma once

#include "framework.hpp"

#include <keeper/keeper.hpp>
#include <collector/source/base.hpp>

namespace collector::feed
{
	//---------------------------------------------------------------------------------------------------------
	/// class linear_generator
	//---------------------------------------------------------------------------------------------------------
	class linear_generator
		: public source::feed
	{
	private:
		using row_t = std::array<double, 2>;	// x, y

	private:
		static constexpr long long row_count_{200};
		static constexpr double a_{0.02};
		static constexpr double b_{0.01};

		std::vector<row_t>	data_;
		std::ptrdiff_t		field_idx_to_store_{-1}; // index in row to store to dest
		std::unique_ptr<keeper::data_write>	dest_;

	public:
		linear_generator(std::string const& field_name);
		//---------------------------------------------------------------------------------------------------------
		// source::feed impl		
		//---------------------------------------------------------------------------------------------------------
		void start(std::unique_ptr<keeper::data_write> dest) override;
		size_t read(const std::span<char> chunk) override;
		void finish(const std::span<char> chunk) override;
		//---------------------------------------------------------------------------------------------------------
		//~source::feed impl		
		//---------------------------------------------------------------------------------------------------------
	};
}