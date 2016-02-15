#pragma once
#include <boost/asio.hpp>
#include <value.hpp>
#include <stream_writer.hpp>

class SenderInterface
{
public:
	using SocketStream = boost::asio::ip::tcp::iostream;

protected:
	SocketStream& stream;

	SenderInterface(SocketStream& stream);

public:
	virtual void send(ubjson::Value val) = 0;
};
