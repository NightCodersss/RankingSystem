#ifndef RANKINGCONNECTION_HPP
#define RANKINGCONNECTION_HPP

#include <string>
#include <memory>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RankingConnection : public std::enable_shared_from_this<RankingConnection>
{
public:
    using pointer = std::shared_ptr<RankingConnection>;
	using ErrorCode = boost::system::error_code;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service);

    void start();

private:
    RankingConnection(boost::asio::io_service& io_service);

	SocketStream south_stream;
	std::vector<std::future<ubjson::Value>> index_results;
};

#endif
