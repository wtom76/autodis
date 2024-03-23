#include "pch.hpp"
#include "mfn.hpp"

//-----------------------------------------------------------------------------------------------------
/// outputs
//-----------------------------------------------------------------------------------------------------
learning::multilayer_feed_forward::outputs::outputs(config const& cfg, double initial_value)
	: layers_(cfg.layer_sizes().size())
	, functions_(cfg.layer_sizes().size() - 1)
{
	layers_.front().resize(cfg.layer_sizes().front(), initial_value);
	layers_.back().resize(cfg.layer_sizes().back(), initial_value);
	functions_.back().resize(cfg.layer_sizes().back(), &tanh_func_);
	for (size_t i = 1; i < cfg.layer_sizes().size() - 1; ++i)
	{
		layers_[i].resize(cfg.layer_sizes()[i] * hidden_func_count_, initial_value);
		functions_[i - 1].resize(cfg.layer_sizes()[i] * hidden_func_count_, nullptr);
		for (size_t aidx{0}; aidx < cfg.layer_sizes()[i]; ++aidx)
		{
			static_assert(hidden_func_count_ == 1);
			functions_[i - 1][aidx] = &sigma_func_;
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void learning::multilayer_feed_forward::outputs::reset(double val)
{
	for (auto& layer : layers_)
	{
		std::fill(std::begin(layer), std::end(layer), val);
	}
}
//-----------------------------------------------------------------------------------------------------
/// biases
//-----------------------------------------------------------------------------------------------------
learning::multilayer_feed_forward::biases::biases(outputs const& cfg, double rand_min, double rand_max)
	: layers_(cfg.layers_.size())
{
	std::random_device rd;
	std::mt19937 rand(rd());
	const double rand_factor = (rand_max - rand_min) / (rand.max() - rand.min());

	layers_.front().resize(cfg.layers_.front().size(), 0.); // input layer isn't used for now
	for (size_t i = 1; i < cfg.layers_.size(); ++i)
	{
		layers_[i].resize(cfg.layers_[i].size()); // 1 chunk for identity and 1 chunk for tanh
		for (auto& hl_val : layers_[i])
		{
			hl_val = rand_min + (rand() - rand.min()) * rand_factor;
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void learning::multilayer_feed_forward::biases::reset(double val)
{
	for (auto& layer : layers_)
	{
		std::fill(std::begin(layer), std::end(layer), val);
	}
}
//-----------------------------------------------------------------------------------------------------
/// weights
//-----------------------------------------------------------------------------------------------------
learning::multilayer_feed_forward::weights::weights(const outputs& cfg, double def)
	: layers_(cfg.layers_.size() - 1)
{
	auto hls = std::cbegin(cfg.layers_);
	auto dest_hls = hls;
	++dest_hls;
	std::for_each(std::begin(layers_), std::end(layers_),
		[&hls, &dest_hls, def](auto& hl)
		{
			hl.resize((hls++)->size(), dest_t((dest_hls++)->size(), def));
		}
	);
}
//-----------------------------------------------------------------------------------------------------
learning::multilayer_feed_forward::weights::weights(const outputs& cfg, double rand_min, double rand_max)
	: layers_(cfg.layers_.size() - 1)
{
	std::random_device rd;
	std::mt19937 rand(rd());
	const double rand_factor = (rand_max - rand_min) / (rand.max() - rand.min());

	auto hls = std::cbegin(cfg.layers_);
	auto dest_hls = hls;
	++dest_hls;
	std::for_each(std::begin(layers_), std::end(layers_),
		[&hls, &dest_hls, &rand, rand_factor, rand_min](auto& hl)
		{
			hl.resize((hls++)->size(), dest_t((dest_hls++)->size()));
			for (auto& hl_val : hl)
			{
				for (auto& dest_val : hl_val)
				{
					dest_val = rand_min + (rand() - rand.min()) * rand_factor;
				}
			}
		}
	);
}
//-----------------------------------------------------------------------------------------------------
void learning::multilayer_feed_forward::weights::reset(double val)
{
	for (auto& layer : layers_)
	{
		for (auto& dest : layer)
		{
			std::fill(std::begin(dest), std::end(dest), val);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
/// multilayer_feed_forward
//-----------------------------------------------------------------------------------------------------
learning::multilayer_feed_forward::multilayer_feed_forward(const config& cfg)
	: outputs_{cfg, 0.}
	, biases_{outputs_, cfg.initial_weight_min_, cfg.initial_weight_max_}
	, weights_{outputs_, cfg.initial_weight_min_, cfg.initial_weight_max_}
{
}
//-----------------------------------------------------------------------------------------------------
learning::multilayer_feed_forward::~multilayer_feed_forward()
{
}
//-----------------------------------------------------------------------------------------------------
void learning::multilayer_feed_forward::forward()
{
	auto src_layer_i = std::begin(outputs_.layers_);
	auto func_layer_i = std::begin(outputs_.functions_); // activation functions for dest layer
	auto dst_layer_i = src_layer_i;
	++dst_layer_i;
	auto dst_layer_e = std::end(outputs_.layers_);

	auto weights_layer_i = std::begin(weights_.layers_);
	auto bias_layer_i = std::begin(biases_.layers_);
	++bias_layer_i;

	for (; dst_layer_i != dst_layer_e;
		++src_layer_i, ++dst_layer_i, ++weights_layer_i, ++bias_layer_i, ++func_layer_i)
	{
		std::vector<double>& src_outputs = *src_layer_i;
		std::vector<double>& dst_outputs = *dst_layer_i;
		std::vector<double>& biases = *bias_layer_i;
		std::vector<activation::function*>& funcs = *func_layer_i;
		const weights::source_to_dest_matrix_t& weights = *weights_layer_i;

		for (size_t dst_idx = 0; dst_idx < dst_outputs.size(); ++dst_idx)
		{
			double net = 0.;
			for (size_t src_idx = 0; src_idx < src_outputs.size(); ++src_idx)
			{
				net += src_outputs[src_idx] * weights[src_idx][dst_idx];
			}
			net += biases[dst_idx];
			dst_outputs[dst_idx] = funcs[dst_idx]->activation(net);
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void learning::to_json(nlohmann::json& j, const multilayer_feed_forward& src)
{
	j = nlohmann::json{
		{ "biases", src.biases_.layers_ },
		{ "weights", src.weights_.layers_ }
	};
}
//---------------------------------------------------------------------------------------------------------
void learning::from_json(const nlohmann::json& j, multilayer_feed_forward& dst)
{
	j.at("biases").get_to(dst.biases_.layers_);
	j.at("weights").get_to(dst.weights_.layers_);
}
