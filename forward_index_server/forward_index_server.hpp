#ifndef FORWARD_INDEXSERVER_HPP
#define FORWARD_INDEXSERVER_HPP

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "forward_index_connection.hpp"
#include "text_forward_index_info.hpp"

/**
 * ForwardIndex server class
 */

class ForwardIndexServer
{
    friend class ForwardIndexConnection;
public:

    ForwardIndexServer(boost::asio::io_service& io_service, int port);

	void inc_connections();
	void dec_connections();

private:

    void start_accept();
    void handle_accept(ForwardIndexConnection::pointer new_connection, const boost::system::error_code& error);

	std::mutex connections_mutex;
	int connections = 0;
    
    tcp::acceptor acceptor;

	std::vector<std::map<std::string, TextForwardIndexInfo>> forward_index;
	void load_forward_index();
};

#endif
