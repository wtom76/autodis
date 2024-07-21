#pragma once
#include "framework.hpp"
#include "data_uri.hpp"

namespace keeper
{
	class config;

	using feed_args_t = shared::util::uri;

	//---------------------------------------------------------------------------------------------------------
	/// class metadata
	//---------------------------------------------------------------------------------------------------------
	class metadata
	{
	public:
		//---------------------------------------------------------------------------------------------------------
		// feed_info
		//---------------------------------------------------------------------------------------------------------
		struct feed_info
		{
			std::string					feed_uri_;
			std::vector<feed_args_t>	feed_args_;
			std::vector<data_uri>		data_uri_;
		};
		//---------------------------------------------------------------------------------------------------------
		// source_info
		//---------------------------------------------------------------------------------------------------------
		struct source_info
		{
			long long	source_id_{0};
			std::string	source_uri_;
			std::string	source_args_;
			bool		pending_{false};	// flag that source has new data to fetch
			bool		always_{false};		// true if pending_ shouldn't be dropped after data acquisition
			feed_info	dest_;
		};
		//---------------------------------------------------------------------------------------------------------
		// data_info
		//---------------------------------------------------------------------------------------------------------
		struct data_info
		{
			long long	data_id_{0};
			data_uri	data_uri_;
			std::string	description_;
		};
		//---------------------------------------------------------------------------------------------------------
		// feature_info
		//---------------------------------------------------------------------------------------------------------
		class feature_info
		{
		private:
			static constexpr std::int32_t type_id_template_{1'000'000};
		public:
			std::int64_t	id_{0};
			std::int32_t	type_id_{0};
			std::string		label_;
			nlohmann::json	formula_;

			bool is_from_db() const noexcept { return id_; }	// the feature info is from the database
			bool is_template() const noexcept;					// the feature info represents a feature template
			std::string_view feature_type() const;
		};

	private:
		std::mutex con_mtx_;
		pqxx::connection con_;

	private:
		std::vector<data_info> _load_data_meta();
		void _read_query_result(pqxx::result const& r, std::vector<feature_info>& dest) const;

	public:
		metadata(config const& cfg);
		[[nodiscard]] std::vector<source_info> load_source_meta();
		[[nodiscard]] std::vector<data_info> load_data_meta(std::vector<long long> const& reg_data_ids);
		void drop_pending_flag(long long source_id);
		[[nodiscard]] std::vector<feature_info> load_feature_meta(std::vector<std::int64_t> const& feature_ids);
		[[nodiscard]] std::vector<std::int64_t> load_feature_ids_by_type(std::int64_t feature_set_id, std::int32_t type_id);
	};

	//---------------------------------------------------------------------------------------------------------
	void to_json(nlohmann::json& j, metadata::feature_info const& src);
	void from_json(const nlohmann::json& j, metadata::feature_info& dst);
}
