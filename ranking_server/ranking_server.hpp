#ifndef RANKINGSERVER_HPP
#define RANKINGSERVER_HPP

#include <string>
#include <memory>
#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <config_loader.hpp>

#include "ranking_connection.hpp"

/**
 * Ranking protocol server class
 */

class RankingServer
{
public:

    RankingServer(boost::asio::io_service& io_service, std::string config_file);

	void inc_connections();
	void dec_connections();

private:

	config_type config;
	
    void start_accept();
    void handle_accept(RankingConnection::pointer new_connection, const boost::system::error_code& error);

	std::mutex connections_mutex;
	int connections = 0;
    
    tcp::acceptor acceptor;
};

#endif
