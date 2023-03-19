#pragma once

#include "framework.hpp"
#include "data_uri.hpp"

#include <pqxx/pqxx>

namespace keeper
{
	class config;

	//---------------------------------------------------------------------------------------------------------
	/// class data_read
	//---------------------------------------------------------------------------------------------------------
	class data_read
	{
	private:
		using table_fields_map_t = std::unordered_map<std::string /*table name*/, std::vector<std::string> /*field list*/>;

	private:
		pqxx::connection con_;

	private:
		table_fields_map_t _field_map(std::vector<data_uri> const& src_uri) const;
		std::string _field_list(std::vector<std::string> const& fields) const;

	public:
		data_read(config const& cfg);

		void read(std::vector<data_uri> const& src_uri, shared::data::frame& dest);
	};
}