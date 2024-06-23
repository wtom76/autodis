#include "pch.hpp"
#include "abstract.hpp"

void feature::abstract::dump(std::filesystem::path const& out_path) const
{
	std::map<index_value_t, value_t> const sorted_data{data_.begin(), data_.end()};
	std::ofstream out{out_path};
	out << "index;value\r\n"sv;
	for (auto const& pr : sorted_data)
	{
		out << pr.first << ';' << pr.second << '\n';
	}
}