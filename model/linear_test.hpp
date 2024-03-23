#pragma once

#include <shared/math/normalization.hpp>
#include "framework.hpp"
#include "common.hpp"
#include "abstract.hpp"
#include "file.hpp"

namespace autodis::model
{
	//---------------------------------------------------------------------------------------------------------
	// class linear_test
	// testing model
	//---------------------------------------------------------------------------------------------------------
	class linear_test
		: public abstract
		, private shared::util::logged
	{
	// types
	private:
		using frame_t = shared::data::frame;
		using norm_t = shared::math::tanh_normalization;
		using norm_container_t = std::vector<norm_t>;
	
		class prediction_context;

	// data
	private:
		std::string const	target_series_name_{"y"s};
		std::string const	predicted_series_name_{"predicted"s};
		file				model_file_;
		frame_t				df_;
		norm_container_t	norm_;
		double				best_err_{0};
		std::shared_ptr<autodis::visual::chart> chart_;
		std::vector<std::string> input_series_names_{"x"s};

	// methods
	private:
		void _load_data();
		void _normalize();
		void _create_chart();
		void _learn();
		std::optional<prediction_result_t> _predict();
		void _show();

	public:
		explicit linear_test(file&& model_file);

		static void create_model_file(
			std::string type_name,
			std::int64_t model_id,
			std::filesystem::path file_path);

		std::int64_t id() override { return 1; }
		void learn() override;
		std::optional<prediction_result_t> predict() override;
		void show() override;
		void show_analysis() override {}
		void show_partial_dependence() override;
	};
}