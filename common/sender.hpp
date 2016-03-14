#pragma once
#include <boost/asio.hpp>
#include <value.hpp>
#include <stream_writer.hpp>

class SenderInterface
{
public:
	using SocketStream = boost::asio::ip::tcp::iostream;
	
	virtual void send(ubjson::Value val) = 0;

public:
	int sent;

protected:
	SenderInterface(SocketStream& stream);
	
	SocketStream& stream;
};
