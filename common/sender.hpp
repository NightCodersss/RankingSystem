#pragma once
#include <boost/asio.hpp>
#include <value.hpp>
#include <stream_writer.hpp>

class SenderInterface
{
public:
	using SocketStream = boost::asio::ip::tcp::iostream;

	virtual void send(SocketStream& stream, ubjson::Value val) = 0;
};
