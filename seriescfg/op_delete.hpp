#pragma once

#include "framework.hpp"
#include "series_params.hpp"

namespace seriescfg
{
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class op_delete
		: private shared::util::logged
	{
	private:
		series_params params_;
	private:
		void _drop_table(pqxx::work& t) const;
		[[nodiscard]] std::array<std::int64_t, 5> _delete_data_registry(pqxx::work& t) const;
		[[nodiscard]] std::array<std::int64_t, 2> _delete_source_binding(pqxx::work& t, std::array<std::int64_t, 5> const& dr_ids) const;
		void _delete_source_registry(pqxx::work& t, std::array<std::int64_t, 2> const& src_ids) const;
	public:
		explicit op_delete(series_params params);
		void run(pqxx::connection& con);
	};
}