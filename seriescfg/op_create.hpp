#pragma once

#include "framework.hpp"
#include "series_params.hpp"

namespace seriescfg
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class op_create
		: private shared::util::logged
	{
	private:
		series_params params_;
	private:
		void _create_table(pqxx::work& t) const;
		[[nodiscard]] std::array<std::int64_t, 5> _fill_data_registry(pqxx::work& t) const;
		[[nodiscard]] std::vector<std::int64_t> _fill_source_registry(pqxx::work& t) const;
		void _fill_source_binding(pqxx::work& t, std::array<std::int64_t, 5> const& dr_ids, std::vector<std::int64_t> const& src_ids) const;
	public:
		explicit op_create(series_params params);
		void run(pqxx::connection& con);
	};
}