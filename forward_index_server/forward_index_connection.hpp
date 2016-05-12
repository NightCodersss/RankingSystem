#ifndef FORWARD_INDEXCONNECTION_HPP
#define FORWARD_INDEXCONNECTION_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <value.hpp>

#include "text_forward_index_info.hpp"

using boost::asio::ip::tcp;

class ForwardIndexServer;

class ForwardIndexConnection : public std::enable_shared_from_this<ForwardIndexConnection>
{
	friend class ForwardIndexServer;
public:
    using pointer = std::shared_ptr<ForwardIndexConnection>;
	using SocketStream = boost::asio::ip::tcp::iostream;

    static pointer create(boost::asio::io_service& io_service, ForwardIndexServer * const server);

    void start();

	~ForwardIndexConnection();

private:
    ForwardIndexConnection(boost::asio::io_service& io_service, ForwardIndexServer * const server);

    SocketStream ranking_stream;

	ForwardIndexServer* const server;
	std::string forward_index_path;
};

#endif
