#pragma once

#include <nlohmann/json.hpp>

#include "common.hpp"
#include "config.hpp"
#include "activation.hpp"

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class multilayer_feed_forward
	//-----------------------------------------------------------------------------------------------------
	class multilayer_feed_forward
	{
	// types
	public:
		using config_t = config;
		struct outputs
		{
		private:
			static constexpr std::size_t hidden_func_count_{3};

			activation::identity	identity_func_;
			activation::sigma		sigma_func_;
			activation::tanh		tanh_func_;
			activation::gaussian	gaussian_func_;
		public:
			std::vector<std::vector<double>> layers_;
			std::vector<std::vector<activation::function*>> functions_;

			outputs() = default;
			outputs(config_t const& cfg, double def);
			void reset(double val);
		};
		struct biases
		{
			std::vector<std::vector<double>> layers_;

			biases() = default;
			biases(outputs const& cfg, double rand_min, double rand_max);
			void reset(double val);
		};
		struct weights
		{
			using dest_t = std::vector<double>; // from one source node to all destination nodes
			using source_to_dest_matrix_t = std::vector<dest_t>;

			std::vector<source_to_dest_matrix_t> layers_;

			weights() = default;
			weights(outputs const& cfg, double def);
			weights(outputs const& cfg, double rand_min, double rand_max);
			void reset(double val);
		};

	// data
	private:
		outputs	outputs_;
		biases	biases_;
		weights	weights_;

	// methods
	public:
		multilayer_feed_forward() = delete;
		explicit multilayer_feed_forward(config_t const& cfg);
		~multilayer_feed_forward();

		std::vector<double>& input_layer() noexcept { return outputs_.layers_.front(); }
		std::vector<double> const& omega_layer() const noexcept { return outputs_.layers_.back(); }
		std::vector<std::vector<double>> const& output_layers() const noexcept { return outputs_.layers_; }
		std::vector<std::vector<double>> const& bias_layers() const noexcept { return biases_.layers_; }
		std::vector<std::vector<double>>& bias_layers() noexcept { return biases_.layers_; }
		double weight(size_t layer, size_t src_node, size_t dst_node) const noexcept { return weights_.layers_[layer][src_node][dst_node]; }
		std::vector<weights::source_to_dest_matrix_t> const& weight_layers() const noexcept { return weights_.layers_; }
		std::vector<weights::source_to_dest_matrix_t>& weight_layers() noexcept { return weights_.layers_; }
		std::vector<std::vector<activation::function*>> const& functions() const noexcept { return outputs_.functions_; }

		void forward();

		friend std::ostream& operator <<(std::ostream& strm, multilayer_feed_forward const& network);
		friend std::istream& operator >>(std::istream& strm, multilayer_feed_forward& network);
		friend void to_json(nlohmann::json& j, const multilayer_feed_forward& src);
		friend void from_json(const nlohmann::json& j, multilayer_feed_forward& dst);
	};
	
	std::ostream& operator <<(std::ostream& strm, multilayer_feed_forward const& network);
	std::istream& operator >>(std::istream& strm, multilayer_feed_forward& network);
	void to_json(nlohmann::json& j, const multilayer_feed_forward& src);
	void from_json(const nlohmann::json& j, multilayer_feed_forward& dst);
}