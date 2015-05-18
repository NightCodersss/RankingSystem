#include "ranking_server.hpp"
    
RankingServer::RankingServer(boost::asio::io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
    start_accept();
}
    
void RankingServer::start_accept()
{
    auto new_connection = RankingConnection::create(acceptor.get_io_service());

    acceptor.async_accept(*new_connection->client_stream.rdbuf(), 
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void RankingServer::handle_accept(RankingConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
        new_connection->start();

    start_accept();
}

