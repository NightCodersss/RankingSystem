#ifndef RANKINGSERVER_HPP
#define RANKINGSERVER_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "ranking_connection.hpp"

/**
 * Ranking protocol server class
 */

class RankingServer
{
public:

    RankingServer(boost::asio::io_service& io_service, std::string config_file);

private:

	ubjson::Value config;
	
    void start_accept();
    void handle_accept(RankingConnection::pointer new_connection, const boost::system::error_code& error);
    
    tcp::acceptor acceptor;
};

#endif
