#ifndef RANKINGCONNECTION_HPP
#define RANKINGCONNECTION_HPP

#include <string>
#include <memory>
#include <vector>
#include <future>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <value.hpp>
#include <config_loader.hpp>
#include <bitset>
#include "sortbyrankgetbyidwithtop.hpp"

using boost::asio::ip::tcp;
using DocID = long long;
using TextID = std::string;

class RankingConnection : public std::enable_shared_from_this<RankingConnection>
{
	friend class RankingServer;
public:

    using pointer = std::shared_ptr<RankingConnection>;
	using ErrorCode = boost::system::error_code;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service, const config_type& config);

    void start();

private:
	struct Doc
	{
		static const int TextCount = 10;

		ubjson::Value doc;
		std::bitset<TextCount> got;
		double mdr = 1;
		void update(json const& config, auto const& c); // TODO cut this shit out
	};
 
	struct RankingSystemData
	{
		RankingSystemData() = default;
		RankingSystemData(config_type const& config);
		std::map<DocID, Doc> docs; // docid, doc
		SortByRankGetByIdWithTop<DocID, double> docs_top {0, 0}; // TODO set top_const, bottom_const
		std::mutex docs_mutex;
		std::mutex mdr_mutex; // is not used
		bool is_end = false; // NOTE: maybe use std::atomic_flag
		int download_counter = 0;
		double Mdr;
		std::map<TextID, double> c;
	};


    RankingConnection(boost::asio::io_service& io_service, const config_type& config);

	const config_type& config;
	RankingSystemData data; // NOTE: defined here but not locally in main lambda becuse it need to be on the heap
	SocketStream south_stream;
	std::vector<std::future<void>> index_results;
};

#endif
