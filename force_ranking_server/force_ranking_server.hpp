#ifndef FORCE_RANKINGSERVER_HPP
#define FORCE_RANKINGSERVER_HPP

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <config_loader.hpp>

#include "force_ranking_connection.hpp"

/**
 * ForceRanking server class
 */

class ForceRankingServer
{
    friend class ForceRankingConnection;
public:

    ForceRankingServer(boost::asio::io_service& io_service, int port, std::string config_file);

	void inc_connections();
	void dec_connections();

private:

	config_type config;

    void start_accept();
    void handle_accept(ForceRankingConnection::pointer new_connection, const boost::system::error_code& error);

	std::mutex connections_mutex;
	int connections = 0;

	std::map<std::string, double> rank_form_by_id;
    
    tcp::acceptor acceptor;
};

#endif
