#ifndef INDEXSERVER_HPP
#define INDEXSERVER_HPP

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <idf.hpp>

#include "index_connection.hpp"
#include "text_index_info.hpp"

/**
 * Index server class
 */

class IndexServer
{
    friend class IndexConnection;
public:

    IndexServer(boost::asio::io_service& io_service, int port);

	void inc_connections();
	void dec_connections();

	IDFHandler idf;

private:

    void start_accept();
    void handle_accept(IndexConnection::pointer new_connection, const boost::system::error_code& error);

	std::mutex connections_mutex;
	int connections = 0;
    
    tcp::acceptor acceptor;
};

#endif
