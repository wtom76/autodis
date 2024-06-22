#pragma once

#include "framework.hpp"

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class config
	//-----------------------------------------------------------------------------------------------------
	class config
	{
	public:
		using layer_sizes_t = std::vector<std::size_t>;

	private:
		layer_sizes_t layer_sizes_;
		bool stable_test_set_{false};	// dataset test set is formed once for all epochs

	public:
		static constexpr double initial_weight_min_{-0.1}; //0.01;
		static constexpr double initial_weight_max_{0.1};

		// provide all layer sizes including input and omega
		explicit config(layer_sizes_t const& layers_sizes)
			: layer_sizes_{layers_sizes}
		{
			if (layer_sizes_.size() < 3)
			{
				throw std::runtime_error{"please configure at least one hidden layer"};
			}
		}

		layer_sizes_t const& layer_sizes() const noexcept { return layer_sizes_; }
		void layer_sizes(layer_sizes_t&& layer_sizes) noexcept { layer_sizes_ = std::move(layer_sizes); }
		bool stable_test_set() const noexcept { return stable_test_set_; }

		friend void to_json(nlohmann::json& j, config const& src);
		friend void from_json(nlohmann::json const& j, config& dst);
	};
	//---------------------------------------------------------------------------------------------------------
	inline void to_json(nlohmann::json& j, config const& src)
	{
		j = nlohmann::json{
			{ "layer_sizes", src.layer_sizes_ },
			{ "stable_test_set", src.stable_test_set_ }
		};
	}
	//---------------------------------------------------------------------------------------------------------
	inline void from_json(nlohmann::json const& j, config& dst)
	{
		j.at("layer_sizes").get_to(dst.layer_sizes_);
		j.at("stable_test_set").get_to(dst.stable_test_set_);
	}
}