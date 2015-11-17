#ifndef RANKINGSERVER_HPP
#define RANKINGSERVER_HPP

#include <string>
#include <memory>
#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/timer/timer.hpp>
#include <fstream>
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

	void log_timer(char const * const message, boost::timer::cpu_timer& t);

private:

	config_type config;
	
    void start_accept();
    void handle_accept(RankingConnection::pointer new_connection, const boost::system::error_code& error);

	std::mutex connections_mutex;
	int connections = 0;

	std::mutex log_timer_mutex;
	std::ofstream log_timer_file;
    
    tcp::acceptor acceptor;
};

#endif
