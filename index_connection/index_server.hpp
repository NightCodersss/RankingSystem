#ifndef INDEXSERVER_HPP
#define INDEXSERVER_HPP

#include <string>
#include <memory>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "index_connection.hpp"
#include "text_index_info.hpp"
/**
 * Index server class
 */

class IndexServer
{
public:

    IndexServer(boost::asio::io_service& io_service, int port);

private:

    void start_accept();
    void handle_accept(IndexConnection::pointer new_connection, const boost::system::error_code& error);
    
    tcp::acceptor acceptor;

	std::vector<std::vector<TextIndexInfo>> index;
	void load_index();
};

#endif
