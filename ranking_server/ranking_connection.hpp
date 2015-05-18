#ifndef RANKINGCONNECTION_HPP
#define RANKINGCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RankingConnection : public std::enable_shared_from_this<RankingConnection>
{
	static constexpr const char* host = "localhost";
	static constexpr const char* port = "14000";

public:
    using pointer = std::shared_ptr<RankingConnection>;
	using ErrorCode = boost::system::error_code;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service);

    tcp::socket& getClientSocket();
	void sendToRankingSystem();

    void start();

//private:
    RankingConnection(boost::asio::io_service& io_service);

    std::string getUBJSONFromQuery(std::string input);

    std::string input;
    
	tcp::socket client;

	SocketStream client_stream;
	SocketStream server_stream;
};

#endif
