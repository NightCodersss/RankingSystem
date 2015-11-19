#include "ranking_server.hpp"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

static const int MAX_CONNECTIONS = 20;
    
RankingServer::RankingServer(boost::asio::io_service& io_service, std::string config_file) : config(ConfigLoader(config_file).get())
																			, acceptor(io_service, tcp::endpoint(tcp::v4(), config["ranking_server"]["port"].get<int>()))
																			, log_timer_file("timing.log")
{
	if ( config["texts"].size() > Doc::TextCount )
	{
		throw std::logic_error("Too many texts. Change constant Doc::TextCount to bigger constant and recompile");
	}

//	std::cout << "Started ranking server\n";
//	std::cout << "Config: \n" << config << '\n';
	BOOST_LOG_TRIVIAL(info) << "Started ranking server\n";
	BOOST_LOG_TRIVIAL(trace) << "Config: \n" << config << '\n';

    start_accept();
}
    
void RankingServer::start_accept()
{
    auto new_connection = RankingConnection::create(acceptor.get_io_service(), config, this);

    acceptor.async_accept(*new_connection->south_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void RankingServer::handle_accept(RankingConnection::pointer new_connection, const boost::system::error_code& error)
{
    if (!error)
	{
//		std::cout << "New ranking connection\n";
		BOOST_LOG_TRIVIAL(trace) << "New ranking connection\n";
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! error caught at handle_accept\n";

	// Waiting for decreasing connections
	int connections_local;
	do
	{
		{
			std::lock_guard<std::mutex> lock(connections_mutex);
			connections_local = connections;
		}
		std::this_thread::yield();
	}
	while (connections_local >= MAX_CONNECTIONS);

    start_accept();
}

void RankingServer::inc_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	++connections;
	BOOST_LOG_TRIVIAL(info) << "Connections uped to: " << connections;
}

void RankingServer::dec_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	--connections;
	BOOST_LOG_TRIVIAL(info) << "Connections downed to: " << connections;
}

void RankingServer::log_timer(char const * const message, boost::timer::cpu_timer& t)
{
	std::lock_guard<std::mutex> lock(log_timer_mutex);
	log_timer_file << message << t.format() << "\n";
	log_timer_file.flush();
}
