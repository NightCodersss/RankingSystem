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
#include "ranking_struct.hpp"
#include "streams_dispatcher.hpp"

#include <defines.hpp>

using boost::asio::ip::tcp;

class RankingServer;

class RankingConnection : public std::enable_shared_from_this<RankingConnection>
{
	friend class RankingServer;
public:

    using pointer = std::shared_ptr<RankingConnection>;
	using ErrorCode = boost::system::error_code;

    static pointer create(boost::asio::io_service& io_service, const config_type& config, RankingServer * const server);

    void start();

	~RankingConnection();
	
private:
    RankingConnection(boost::asio::io_service& io_service, const config_type& config, RankingServer* const server);

	const config_type& config;
	RankingStruct data; // NOTE: defined here but not locally in main lambda becuse it need to be on the heap
	StreamsDispatcher streams_dispatcher;
	SocketStream south_stream;
	std::vector<std::future<void>> index_results;

	RankingServer* const server;
};

#endif
