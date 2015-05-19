#ifndef RANKINGCONNECTION_HPP
#define RANKINGCONNECTION_HPP

#include <string>
#include <memory>
#include <vector>
#include <future>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "../UbjsonCpp/include/value.hpp"
#include "../UbjsonCpp/include/stream_reader.hpp"
#include "../UbjsonCpp/include/stream_writer.hpp"
#include "../config_loader.hpp"

using boost::asio::ip::tcp;

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
    RankingConnection(boost::asio::io_service& io_service, const config_type& config);

	const config_type& config;
	SocketStream south_stream;
	std::vector<std::future<ubjson::Value>> index_results;
};

#endif
