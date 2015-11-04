#include "south_server.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <thread>

static const int MAX_CONNECTIONS = 20;
    
SouthServer::SouthServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	connections = 0;
	BOOST_LOG_TRIVIAL(trace) << "Started south server on port " << port << '\n';
    start_accept();
}
    
void SouthServer::start_accept()
{
    auto new_connection = SouthConnection::create(acceptor.get_io_service(), this);

    acceptor.async_accept(*new_connection->client_stream.rdbuf(), 
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void SouthServer::handle_accept(SouthConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
		BOOST_LOG_TRIVIAL(trace) << "New south connection\n";
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! error caught at handle_accept\n";
	
	do
	{
		std::this_thread::yield();
	}
	while (connections.load() >= MAX_CONNECTIONS);

    start_accept();
}

void SouthServer::inc_connections()
{
	++connections;
	BOOST_LOG_TRIVIAL(info) << "Connections uped to: " << connections.load() << std::endl;
}

void SouthServer::dec_connections()
{
	--connections;
	BOOST_LOG_TRIVIAL(info) << "Connections downed : " << connections.load() << std::endl;
}
