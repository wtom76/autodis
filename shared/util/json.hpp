#pragma once

#include "framework.hpp"

namespace shared::util
{
	using nlohmann::json;

	//---------------------------------------------------------------------------------------------------------
	template <typename value_t>
	void to_json(json& j, json::object_t::key_type const& field_name, std::optional<value_t> const& src)
	{
		if (src)
		{
			j[field_name] = *src;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	template <typename value_t>
	void to_json(json& j, json::object_t::key_type const& field_name, value_t const& null_value, value_t const& src)
	{
		if (src != null_value)
		{
			j[field_name] = src;
		}
	}
	//---------------------------------------------------------------------------------------------------------
	template <typename value_t>
	[[nodiscard]] std::optional<value_t> optional_from_json(json const& j, json::object_t::key_type const& field_name)
	{
		if (auto const iter{j.find(field_name)}; iter != j.cend())
		{
			return iter->get<value_t>();
		}
		return {};
	}
	//---------------------------------------------------------------------------------------------------------
	template <typename value_t, typename default_value_t>
	[[nodiscard]] value_t from_json(json const& j, json::object_t::key_type const& field_name, default_value_t&& default_value)
	{
		if (auto const iter{j.find(field_name)}; iter != j.cend())
		{
			return iter->get<value_t>();
		}
		return std::forward<default_value_t>(default_value);
	}
	//---------------------------------------------------------------------------------------------------------
	template <typename value_t>
	void from_json_to(json const& j, json::object_t::key_type const& field_name, std::optional<value_t>& dest)
	{
		if (auto const iter{j.find(field_name)}; iter != j.cend())
		{
			dest = iter->get<value_t>();
		}
		else
		{
			dest.reset();
		}
	}
	//---------------------------------------------------------------------------------------------------------
	template <typename value_t, typename default_value_t>
	void from_json_to(json const& j, json::object_t::key_type const& field_name, default_value_t&& default_value, value_t& dest)
	{
		if (auto const iter{j.find(field_name)}; iter != j.cend())
		{
			iter->get_to(dest);
		}
		else
		{
			dest = std::forward<default_value_t>(default_value);
		}
	}
}