#ifndef SOUTHCONNECTION_HPP
#define SOUTHCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// TODO break connection with server if connection with client has broken

class SouthServer;

class SouthConnection : public std::enable_shared_from_this<SouthConnection>
{
	static constexpr const char* host = "localhost";
	static constexpr const char* port = "14000";

	friend class SouthServer; // To make client_stream.rdbuf() visible to server

public:
    using pointer = std::shared_ptr<SouthConnection>;
	using ErrorCode = boost::system::error_code;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service, SouthServer* const server);

    tcp::socket& getClientSocket();
	void sendToRankingSystem();

    void start();

	~SouthConnection();

private:
    SouthConnection(boost::asio::io_service& io_service, SouthServer* const server);

    std::string getUBJSONFromQuery(std::string input);

    std::string input;
    
	tcp::socket client;

	SocketStream client_stream;
	SocketStream server_stream;

	SouthServer* const server;
};

#endif
