#include "ranking_server.hpp"
    
RankingServer::RankingServer(boost::asio::io_service& io_service, std::string config_file) : config(ConfigLoader(config_file).get())
																			, acceptor(io_service, tcp::endpoint(tcp::v4(), config["ranking_server"]["port"].get<int>()))
{
	std::cout << "Started ranking server\n";
	std::cout << "Config: \n" << config << '\n';
	std::cerr << "Started ranking server\n";
	std::cerr << "Config: \n" << config << '\n';

    start_accept();
}
    
void RankingServer::start_accept()
{
    auto new_connection = RankingConnection::create(acceptor.get_io_service(), config);

    acceptor.async_accept(*new_connection->south_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void RankingServer::handle_accept(RankingConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
		std::cout << "New ranking connection\n";
		std::cerr << "New ranking connection\n";
        new_connection->start();
	}
	else
		std::cerr << "!!!!! error caught at handle_accept\n";

    start_accept();
}

