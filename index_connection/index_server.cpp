#include "index_server.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
    
IndexServer::IndexServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	BOOST_LOG_TRIVIAL(info) << "Started index server on port: " << port << '\n';
    start_accept();
}
    
void IndexServer::start_accept()
{
    auto new_connection = IndexConnection::create(acceptor.get_io_service());

    acceptor.async_accept(*new_connection->ranking_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void IndexServer::handle_accept(IndexConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
		BOOST_LOG_TRIVIAL(info) << "New index connection\n";
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! Error at accepting";

    start_accept();
}

