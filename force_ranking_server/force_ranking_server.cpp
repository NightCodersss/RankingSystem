#include "force_ranking_server.hpp"

#include <fstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

ForceRankingServer::ForceRankingServer(boost::asio::io_service& io_service, int port, std::string config_file) 
	: config(ConfigLoader(config_file).get())
	, acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	BOOST_LOG_TRIVIAL(info) << "Started force_ranking server on port: " << port << '\n';
	BOOST_LOG_TRIVIAL(trace) << "Config: \n" << config << '\n';

    start_accept();
}
    
void ForceRankingServer::start_accept()
{
    auto new_connection = ForceRankingConnection::create(acceptor.get_io_service(), this);

    acceptor.async_accept(*new_connection->ranking_stream.rdbuf(), // socket of stream
          [this, new_connection](const boost::system::error_code& error_code) { 
            handle_accept(new_connection, error_code);
          });
}
    
void ForceRankingServer::handle_accept(ForceRankingConnection::pointer new_connection, const boost::system::error_code& err)
{
    if (!err)
	{
		BOOST_LOG_TRIVIAL(info) << "New force_ranking connection\n";
        new_connection->start();
	}
	else
		BOOST_LOG_TRIVIAL(error) << "!!!!! Error at accepting: " << err;

    start_accept();
}

void ForceRankingServer::inc_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	++connections;
	BOOST_LOG_TRIVIAL(info) << "Connections uped to: " << connections;
}

void ForceRankingServer::dec_connections()
{
	std::lock_guard<std::mutex> lock(connections_mutex);
	--connections;
	BOOST_LOG_TRIVIAL(info) << "Connections downed to: " << connections;
}

