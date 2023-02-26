#pragma once

#include <random>
#include <shared/shared.hpp>
#include "common.hpp"
#include "progress_view.hpp"

//-----------------------------------------------------------------------------------------------------
/// ProgessView should provide
///  bool stop_requested() const noexcept { return stop_flag_; }
///  void begin_teach();
///  void set_best(double cur_min_err);
///  void set_last(double err);
///  void end_teach();
///  void begin_test();
///  void add_sample_result(double omega, double target_i);
///  void end_test(double true_count, double false_count);
///  void begin_teach();

namespace learning
{
	//-----------------------------------------------------------------------------------------------------
	// class rprop
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	class rprop
	{
	// types
	public:
		using grad_terms_t		= typename net::outputs;
		using weight_grads_t	= typename net::weights;
		using bias_grads_t		= typename net::outputs;
		using weight_deltas_t	= typename net::weights;
		using bias_deltas_t		= typename net::outputs;

		using data_frame_t		= shared::data::frame;
		using data_view_t		= shared::data::view;

	private:
		struct sample_filler
		{
			const data_view_t& view_;
			std::vector<const data_frame_t::series_t*> inputs_;
			std::vector<const data_frame_t::series_t*> targets_;

			explicit sample_filler(const std::pair<data_frame_t, data_view_t>& dfv,
				const std::vector<std::string>& in_names, const std::vector<std::string>& target_names);
			void fill(std::ptrdiff_t row, std::vector<double>& inputs, std::vector<double>& targets) const;
		};

	// data
	private:
		static constexpr double initial_learning_rate_{0.1};
		static constexpr double min_learning_rate_{1e-6};
		static constexpr double max_learning_rate_{50};
		static constexpr double inc_learning_rate_{1.2};
		static constexpr double dec_learning_rate_{0.5};

		std::unique_ptr<grad_terms_t>		grad_terms_;
		std::unique_ptr<weight_grads_t>		dEdw_off_;
		std::unique_ptr<weight_grads_t>		prev_dEdw_off_;
		std::unique_ptr<bias_grads_t>		bias_dEdw_off_;
		std::unique_ptr<bias_grads_t>		prev_bias_dEdw_off_;
		std::unique_ptr<weight_deltas_t>	prev_weight_deltas_;
		std::unique_ptr<bias_deltas_t>		prev_bias_deltas_;

		std::random_device					rd_;
		std::mt19937						rand_;
		std::pair<data_frame_t, data_view_t>		src_data_;
		std::vector<std::ptrdiff_t>			teaching_set_;
		std::vector<std::ptrdiff_t>			test_set_;
		sample_filler						sample_filler_;
		std::vector<double>					sample_targets_;

	// methods
	private:
		void _init(const typename net::config& cfg);
		void _updateGradients(net& network, const std::vector<double>& targets);
		void _updateWeights(net& network);
		void _updateBiases(net& network);
		void _splitData();
		double _meanSqrError(net& network);
	public:
		rprop(std::pair<data_frame_t, data_view_t>&& src_data,
			const std::vector<std::string>& in_names,
			const std::vector<std::string>& target_names)
			: rand_{rd_()}
			, src_data_{std::move(src_data)}
			, sample_filler_{src_data_, in_names, target_names}
			, sample_targets_(target_names.size(), 0.)
		{}

		double teach(const typename net::config& cfg, net& network, double min_err, progress_view& progress_view);
		void show_test(net& network, progress_view& progress_view);
	};
	//-----------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	rprop<net>::sample_filler::sample_filler(const std::pair<data_frame_t, data_view_t>& dfv,
		const std::vector<std::string>& in_names,
		const std::vector<std::string>& target_names)
		: view_{dfv.second}
		, inputs_(in_names.size(), nullptr)
		, targets_(target_names.size(), nullptr)
	{
		for (size_t i = 0; i != in_names.size(); ++i)
		{
			inputs_[i] = &dfv.first.series(in_names[i]);
		}
		for (size_t i = 0; i != target_names.size(); ++i)
		{
			targets_[i] = &dfv.first.series(target_names[i]);
		}
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	void rprop<net>::sample_filler::fill(std::ptrdiff_t row, std::vector<double>& inputs, std::vector<double>& targets) const
	{
		auto input_i = std::begin(inputs);
		for (const auto src_input : inputs_)
		{
			++*input_i = (*src_input)[view_.frame_index(row)];
		}
		auto target_i = std::begin(targets);
		for (const auto src_target : targets_)
		{
			++*target_i = (*src_target)[view_.frame_index(row)];
		}
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	void rprop<net>::_init(const typename net::config& cfg)
	{
		grad_terms_			= std::make_unique<grad_terms_t>(cfg, 0.);
		dEdw_off_			= std::make_unique<weight_grads_t>(*grad_terms_, 0.);
		prev_dEdw_off_		= std::make_unique<weight_grads_t>(*grad_terms_, 0.);
		bias_dEdw_off_		= std::make_unique<bias_grads_t>(cfg, 0.);
		prev_bias_dEdw_off_	= std::make_unique<bias_grads_t>(cfg, 0.);
		prev_weight_deltas_	= std::make_unique<weight_deltas_t>(*grad_terms_, initial_learning_rate_);
		prev_bias_deltas_	= std::make_unique<bias_deltas_t>(cfg, initial_learning_rate_);
	}
	//-----------------------------------------------------------------------------------------------------
	// 1. calc gradient terms for omega layer
	// 2. calc gradient terms for hidden layers
	// 3. calc bias gradients for omega layer and hidden layers
	// 4. calc weight gradients for omega layer and hidden layers
	template <class net>
	void rprop<net>::_updateGradients(net& network, const std::vector<double>& targets)
	{
		const size_t layer_count = network.outputs().size();
		assert(layer_count >= 3); // input, n * hidden, omega

		std::int64_t layer_idx{layer_count - 1};
		// 1.
		{
			auto& grad_terms = grad_terms_->layers_.back();
			auto& outputs = network.outputs().back();
			auto& functions = network.functions().back();
			for (size_t node_idx = 0; node_idx < grad_terms.size(); ++node_idx)
			{
				grad_terms[node_idx] =
					functions[node_idx]->derivative(outputs[node_idx]) * (outputs[node_idx] - targets[node_idx]);
			}
		}
		// 2.
		for (--layer_idx; layer_idx >= 1; --layer_idx)
		{
			auto& grad_terms = grad_terms_->layers_[layer_idx];
			auto& prev_grad_terms = grad_terms_->layers_[layer_idx + 1];
			auto& weights = network.weights()[layer_idx];
			auto& outputs = network.outputs()[layer_idx];
			auto& functions = network.functions()[layer_idx - 1];
			for (size_t src_node_idx = 0; src_node_idx < grad_terms.size(); ++src_node_idx)
			{
				double grad_sum = 0.;
				for (size_t dst_node_idx = 0; dst_node_idx < prev_grad_terms.size(); ++dst_node_idx)
				{
					grad_sum += prev_grad_terms[dst_node_idx] * weights[src_node_idx][dst_node_idx];
				}
				grad_terms[src_node_idx] = functions[src_node_idx]->derivative(outputs[src_node_idx]) * grad_sum;
			}
		}

		// 3.
		for (layer_idx = layer_count - 1; layer_idx > 0; --layer_idx)
		{
			auto& bias_grads = bias_dEdw_off_->layers_[layer_idx];
			auto& grad_terms = grad_terms_->layers_[layer_idx];
			for (size_t node_idx = 0; node_idx < bias_grads.size(); ++node_idx)
			{
				bias_grads[node_idx] += grad_terms[node_idx];
			}
		}
		// 4.
		for (layer_idx = layer_count - 2; layer_idx >= 0; --layer_idx)
		{
			auto& dEdw_off = dEdw_off_->layers_[layer_idx];
			auto& outputs = network.outputs()[layer_idx];
			auto& grad_terms = grad_terms_->layers_[layer_idx + 1];
			for (size_t src_node_idx = 0; src_node_idx < dEdw_off.size(); ++src_node_idx)
			{
				for (size_t dst_node_idx = 0; dst_node_idx < dEdw_off[src_node_idx].size();
					++dst_node_idx)
				{
					dEdw_off[src_node_idx][dst_node_idx]
						+= grad_terms[dst_node_idx] * outputs[src_node_idx];
				}
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	void rprop<net>::_updateWeights(net& network)
	{
		const size_t layer_count = network.outputs().size();
		assert(layer_count >= 3); // input, n * hidden, omega

		for (size_t layer_idx = 0; layer_idx < layer_count - 1; ++layer_idx)
		{
			auto& weights = network.weights()[layer_idx];
			auto& prev_weight_deltas = prev_weight_deltas_->layers_[layer_idx];
			auto& dEdw_off = dEdw_off_->layers_[layer_idx];
			auto& prev_dEdw_off = prev_dEdw_off_->layers_[layer_idx];

			for (size_t src_node_idx = 0; src_node_idx < weights.size(); ++src_node_idx)
			{
				for (size_t dst_node_idx = 0; dst_node_idx < weights[src_node_idx].size(); ++dst_node_idx)
				{
					const double prev_weight_delta = prev_weight_deltas[src_node_idx][dst_node_idx];
					double weight_delta = 0.;
					const double weight_grad_prod = prev_dEdw_off[src_node_idx][dst_node_idx] * dEdw_off[src_node_idx][dst_node_idx];

					if (weight_grad_prod > 0.)
					{
						weight_delta = std::min(prev_weight_delta * inc_learning_rate_, max_learning_rate_);
						weights[src_node_idx][dst_node_idx] -= std::copysign(weight_delta, dEdw_off[src_node_idx][dst_node_idx]);
					}
					else if (weight_grad_prod < 0.)
					{
						weight_delta = std::max(prev_weight_delta * dec_learning_rate_, min_learning_rate_);
						weights[src_node_idx][dst_node_idx] -= prev_weight_deltas[src_node_idx][dst_node_idx];
						dEdw_off[src_node_idx][dst_node_idx] = 0.;
					}
					else
					{
						weight_delta = prev_weight_delta;
						weights[src_node_idx][dst_node_idx] -= std::copysign(weight_delta, dEdw_off[src_node_idx][dst_node_idx]);
					}
					assert(weight_delta >= 0.);
					prev_weight_deltas[src_node_idx][dst_node_idx] = weight_delta;
					prev_dEdw_off[src_node_idx][dst_node_idx] = dEdw_off[src_node_idx][dst_node_idx];
				}
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	void rprop<net>::_updateBiases(net& network)
	{
		const size_t layer_count = network.outputs().size();
		assert(layer_count >= 3); // input, n * hidden, omega

		for (size_t layer_idx = 1; layer_idx < layer_count; ++layer_idx)
		{
			auto& biases = network.biases()[layer_idx];
			auto& prev_bias_deltas = prev_bias_deltas_->layers_[layer_idx];
			auto& bias_dEdw_off = bias_dEdw_off_->layers_[layer_idx];
			auto& prev_bias_dEdw_off = prev_bias_dEdw_off_->layers_[layer_idx];

			for (size_t node_idx = 0; node_idx < biases.size(); ++node_idx)
			{
				const double prev_weight_delta = prev_bias_deltas[node_idx];
				double delta = 0.;
				const double weight_grad_prod = prev_bias_dEdw_off[node_idx] * bias_dEdw_off[node_idx];

				if (weight_grad_prod > 0.)
				{
					delta = std::min(prev_weight_delta * inc_learning_rate_, max_learning_rate_);
					biases[node_idx] -= std::copysign(delta, bias_dEdw_off[node_idx]);
				}
				else if (weight_grad_prod < 0.)
				{
					delta = std::max(prev_weight_delta * dec_learning_rate_, min_learning_rate_);
					biases[node_idx] -= prev_bias_deltas[node_idx];
					bias_dEdw_off[node_idx] = 0.;
				}
				else
				{
					delta = prev_weight_delta;
					biases[node_idx] -= std::copysign(delta, bias_dEdw_off[node_idx]);
				}
				assert(delta >= 0.);
				prev_bias_deltas[node_idx] = delta;
				prev_bias_dEdw_off[node_idx] = bias_dEdw_off[node_idx];
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	void rprop<net>::_splitData()
	{
		constexpr double train_frac = 0.8;

		teaching_set_.clear();
		test_set_.clear();

		const size_t src_row_count = src_data_.second.row_count();
		const size_t teaching_size = src_row_count * train_frac;
		if (!teaching_size || teaching_size > src_row_count)
		{
			return;
		}
		teaching_set_.reserve(teaching_size);
		test_set_.reserve(src_row_count - teaching_size);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<size_t> distribution(0, src_row_count - 1);

		for (size_t i = 0; i < src_row_count; ++i)
		{
			if (distribution(gen) < teaching_size)
			{
				teaching_set_.push_back(i);
			}
			else
			{
				test_set_.push_back(i);
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	double rprop<net>::_meanSqrError(net& network)
	{
		if (test_set_.empty())
		{
			return 0.;
		}

		double sqr_err_sum = 0.;
		for (const size_t row : test_set_)
		{
			sample_filler_.fill(row, network.inputLayer(), sample_targets_);
			network.forward();
			auto target_i = cbegin(sample_targets_);
			for (auto omega : network.omegaLayer())
			{
				const double err = (omega - *target_i);
				sqr_err_sum += err * err;
				++target_i;
			}
		}
		return sqr_err_sum / test_set_.size();
	}
	//-----------------------------------------------------------------------------------------------------
	/// \returns best error
	template <class net>
	double rprop<net>::teach(const typename net::config& cfg, net& network, double min_err, progress_view& progress)
	{
		constexpr std::int64_t epochs_max{10000};

		std::stop_token stop_token{progress.stop_token()};
		progress.begin_teach();

		net best_network{network};

		_init(cfg);

		double err{std::numeric_limits<double>::max()};
		double cur_min_err{std::numeric_limits<double>::max()};

		std::int64_t epochs_left{epochs_max};

		while (epochs_left-- && err > min_err && !stop_token.stop_requested())
		{
			_splitData();

			dEdw_off_->reset(0.);
			bias_dEdw_off_->reset(0.);

			const size_t teaching_size{teaching_set_.size()};
			for (size_t row : teaching_set_)
			{
				sample_filler_.fill(row, network.inputLayer(), sample_targets_);
				network.forward();
				_updateGradients(network, sample_targets_);
			}
			_updateWeights(network);
			_updateBiases(network);
			err = std::sqrt(_meanSqrError(network));

			if (cur_min_err > err)
			{
				cur_min_err = err;
				best_network = network;
				progress.set_best(cur_min_err);
			}

			progress.set_last(err);
			progress.set_epoch(epochs_max - epochs_left);
		}

		network = best_network;

		progress.end_teach();

		return cur_min_err;
	}
	//-----------------------------------------------------------------------------------------------------
	template <class net>
	void rprop<net>::show_test(net& network, progress_view& progress)
	{
		progress.begin_test();

		std::size_t true_count{0};
		std::size_t false_count{0};

		for (std::size_t row : test_set_)
		{
			sample_filler_.fill(row, network.inputLayer(), sample_targets_);
			network.forward();
			auto target_i{cbegin(sample_targets_)};
			for (auto omega : network.omega_layer())
			{
				progress.add_sample_result(omega, *target_i);
				//if (omega * *target_i > 0. || omega == 0. && *target_i == 0.)
				if (omega * *target_i > 0.)
				{
					++true_count;
				}
				else if (omega * *target_i < 0.)
				{
					++false_count;
				}
				++target_i;
			}
		}
		progress.end_test(true_count, false_count);
	}
}