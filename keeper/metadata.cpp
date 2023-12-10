#include "pch.hpp"
#include "metadata.hpp"
#include "config.hpp"

//---------------------------------------------------------------------------------------------------------
keeper::metadata::metadata(const config& cfg)
	: con_{cfg.db_connection_}
{
	// drop pending flag
	con_.prepare("dpf", "call \"metadata\".drop_pending_flag($1)");
}
//---------------------------------------------------------------------------------------------------------
std::vector<keeper::metadata::source_info> keeper::metadata::load_source_meta()
{
	std::vector<source_info> result;
	{
		std::unordered_map<long long/*source id*/, source_info> source_map;

		{
			pqxx::work t{con_};
			const pqxx::result r{t.exec_params("select source_id, source_uri, source_args, feed_uri, feed_args, data_uri, pending from metadata.metadata_view")};
			if (r.empty())
			{
				return {};
			}
			for (const pqxx::row& rec : r)
			{
				long long	source_id{rec[0].as<long long>()};
				std::string	suri{rec[1].as<std::string>()};
				std::string	sargs{rec[2].as<std::string>({})};
				std::string	furi{rec[3].as<std::string>()};
				std::string	fargs{rec[4].as<std::string>()};
				data_uri	duri{rec[5].as<std::string>()};
				bool		pending{rec[6].as<bool>()};

				source_info& source{source_map[source_id]};
				source.source_id_ = source_id;
				source.source_uri_ = std::move(suri);
				source.source_args_ = std::move(sargs);
				source.pending_ = pending;

				feed_info& feed{source.dest_};
				if (feed.feed_uri_.empty())
				{
					feed.feed_uri_ = std::move(furi);
				}
				feed.feed_args_.emplace_back(std::move(fargs));
				feed.data_uri_.emplace_back(std::move(duri));
			}
		}
		result.reserve(source_map.size());
		for (auto& source_pr : source_map)
		{
			result.emplace_back(std::move(source_pr.second));
		}
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::vector<keeper::metadata::data_info> keeper::metadata::_load_data_meta()
{
	std::vector<data_info> result;
	{
		pqxx::work t{con_};
		const pqxx::result r{t.exec_params("select data_id, data_uri, data_description from metadata.metadata_view")};
		if (r.empty())
		{
			return {};
		}
		result.reserve(r.size());
		for (const pqxx::row& rec : r)
		{
			long long	data_id{rec[0].as<long long>()};
			data_uri	duri{rec[1].as<std::string>()};
			std::string	descr{rec[2].as<std::string>()};
			result.emplace_back(data_id, std::move(duri), std::move(descr));
		}
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
std::vector<keeper::metadata::data_info> keeper::metadata::load_data_meta(std::vector<long long> const& reg_data_ids)
{
	std::vector<keeper::metadata::data_info> result;
	std::vector<data_info> data{_load_data_meta()};
	std::unordered_map<long long /*data_id*/, data_info> all_data_map;
	for (auto& entry : data)
	{
		all_data_map.emplace(entry.data_id_, entry);
	}
	result.reserve(reg_data_ids.size());
	for (long long requested_id : reg_data_ids)
	{
		//result.emplace_back(std::move(all_data_map[requested_id]));
		result.emplace_back(all_data_map[requested_id]);
	}
	return result;
}
//---------------------------------------------------------------------------------------------------------
void keeper::metadata::drop_pending_flag(long long series_id)
{
	pqxx::work t{con_};
	t.exec_prepared("dpf", series_id);
	t.commit();
}
