#ifndef SOUTHSERVER_HPP
#define SOUTHSERVER_HPP

#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "south_connection.hpp"

/**
 * South protocol server class
 */

class SouthServer
{
public:

    SouthServer(boost::asio::io_service& io_service, int port = 15000);

private:

    void start_accept();
    void handle_accept(SouthConnection::pointer new_connection, const boost::system::error_code& error);
    
    tcp::acceptor acceptor;
};

#endif
