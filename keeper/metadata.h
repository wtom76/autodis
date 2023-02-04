#pragma once

namespace keeper
{
	class config;

	//---------------------------------------------------------------------------------------------------------
	/// class metadata
	//---------------------------------------------------------------------------------------------------------
	class metadata
	{
	public:
		struct data_info
		{
			long long	id_{0};
			std::string	data_uri_;
			std::string	feed_uri_;
			std::string	source_uri_;
		};

	private:
		pqxx::connection con_;

	public:
		metadata(const config& cfg);
		data_info load_series_info(long long id);
		std::vector<data_info> load_all_series_info();
	};
}