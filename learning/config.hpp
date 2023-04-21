#pragma once

#include "framework.hpp"

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class config
	//-----------------------------------------------------------------------------------------------------
	class config
	{
		std::vector<size_t> layer_sizes_;

	public:
		static constexpr double initial_weight_min_{-0.1}; //0.01;
		static constexpr double initial_weight_max_{0.1};

		// provide all layer sizes including input and omega
		explicit config(std::vector<std::size_t> const& layers_sizes)
			: layer_sizes_{layers_sizes}
		{
			assert(layer_sizes_.size() >= 3);
		}

		std::vector<std::size_t> const& layer_sizes() const noexcept { return layer_sizes_; }
	};
}