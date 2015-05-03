#include "index_server.hpp"
    
IndexServer::IndexServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
    start_accept();
}
    
void IndexServer::start_accept()
{
    auto new_connection = IndexConnection::create(acceptor.get_io_service());

    acceptor.async_accept(new_connection->socket(), 
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void IndexServer::handle_accept(IndexConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
        new_connection->start();

    start_accept();
}

