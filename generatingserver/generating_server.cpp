#include "index_server.hpp"
    
GeneratingServer::GeneratingServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	std::cout << "Started index server on port: " << port << '\n';
    start_accept();
}
    
void GeneratingServer::start_accept()
{
    auto new_connection = GeneratingConnection::create(acceptor.get_io_service());

    acceptor.async_accept(*new_connection->ranking_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void GeneratingServer::handle_accept(GeneratingConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
		std::cout << "New index connection\n";
        new_connection->start();
	}

    start_accept();
}

