#pragma once

#include "framework.hpp"

namespace shared::util
{
	//---------------------------------------------------------------------------------------------------------
	// class field_map
	//---------------------------------------------------------------------------------------------------------
	class field_map
	{
	private:
		static constexpr std::size_t idx_null_{std::numeric_limits<std::size_t>::max()};

	public:
		struct name_name
		{
			std::string src_;
			std::string dst_;
		};
		struct name_idx
		{
			std::string src_;
			std::size_t idx_{idx_null_};
		};

	private:
		std::vector<name_idx>		src_name_dst_idx_;
		std::size_t					index_field_src_idx_{idx_null_};		// index of index field in source
		std::string const			index_src_name_;
		std::vector<std::size_t>	src_idx_dst_idx_;						// can contain idx_null

	private:
		//---------------------------------------------------------------------------------------------------------
		static std::vector<name_idx> _build_src_name_dst_idx(
			std::vector<name_name> const& src_dest_names,
			std::ranges::range auto const& dest_names)
		{
			std::vector<name_idx> src_name_dst_idx;
			src_name_dst_idx.reserve(dest_names.size());

			for (std::size_t dst_idx{0}; dst_idx != dest_names.size(); ++dst_idx)
			{
				auto const iter{std::ranges::find(src_dest_names, dest_names[dst_idx], [](auto const& pr) noexcept -> std::string const& { return pr.dst_; })};
				if (iter != src_dest_names.cend())
				{
					src_name_dst_idx.emplace_back(iter->src_, dst_idx);
				}
				else
				{
					throw std::runtime_error("requested field is unknown: \""s + dest_names[dst_idx] + '\"');
				}
			}
			return src_name_dst_idx;
		}
	public:
		//---------------------------------------------------------------------------------------------------------
		explicit field_map(
			std::vector<name_name> const& src_dest_names,
			std::ranges::range auto const& dest_names,
			std::string index_src_name)
			: src_name_dst_idx_{_build_src_name_dst_idx(src_dest_names, dest_names)}
			, index_src_name_{std::move(index_src_name)}
		{}

		void map_next_src_name(std::string const& src_name);
		void validate() const;

		std::size_t field_num() const noexcept { return src_name_dst_idx_.size(); }	// without index
		std::size_t dst_idx(std::size_t src_idx) const noexcept;
		std::size_t index_field_src_idx() const noexcept { return index_field_src_idx_; }

		static constexpr std::size_t null() noexcept { return idx_null_; }
	};
}